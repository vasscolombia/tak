import 'dart:async';
import 'dart:convert';
import 'dart:ffi';
import 'dart:typed_data';
import 'package:ffi/ffi.dart';
import 'package:http/http.dart' as http;
import 'package:tak/tls/tls_connection.dart';

class TakHttpClient extends http.BaseClient {
  static const int DEFAULT_TIMEOUT = 10000;
  static const String HOST_HEADER_KEY = 'Host';
  static const String CONTENT_LENGTH_KEY = 'Content-Length';
  static const String LINE_BREAK = '\r\n';
  static const String HTTP_VERSION = 'HTTP/1.1';
  static const int DEFAULT_READ_SIZE = 4096;
  static const String TRANSFER_ENCODING_KEY = 'Transfer-Encoding';
  static const String CHUNKED_ENCODING = 'chunked';

  final Map<String, TlsConnection> tlsConnections = {};
  int? finalContentLength;

  @override
  Future<http.StreamedResponse> send(http.BaseRequest request) async {
    final uri = request.url;

    // Construct request line
    final requestLine = _constructRequestLine(request.method, uri);

    // Host header
    final hostHeader = '${HOST_HEADER_KEY}: ${uri.host}:${uri.port}';

    // Retrieve or create a TLS connection
    final connectionKey = '${uri.host}:${uri.port}';
    tlsConnections.putIfAbsent(
      connectionKey,
      () => TlsConnection(
        fqdn: uri.host,
        port: uri.port.toString(),
        timeout: DEFAULT_TIMEOUT,
      ),
    );
    final tlsConnection = tlsConnections[connectionKey]!;

    // Add the request body string, if any and add Content-length to the headers
    String body = "";
    if (request is http.Request && request.body.isNotEmpty) {
      // Add content-lenght size
      request.headers.addAll({'Content-Length': '${request.body.length}'});
      body += '${request.body}${LINE_BREAK}$LINE_BREAK';
    }

    // Prepare headers
    final headersBuffer = _buildHeadersBuffer(request.headers);

    // Combine request components
    String httpRequest =
        '$requestLine${LINE_BREAK}$hostHeader${LINE_BREAK}$headersBuffer${LINE_BREAK}$body';

    // Send request
    await tlsConnection.write(httpRequest.toNativeUtf8().cast<Char>());

    // Read and process response
    final responseBytes = await _readResponse(tlsConnection);
    final headersEndIndex = await _findHeadersEndIndex(responseBytes);

    if (headersEndIndex == null) {
      throw Exception(
          "HttpClient: Headers could not be fully parsed for request to ${uri.host}");
    }

    // Parse response headers
    final responseHeadersString =
        utf8.decode(responseBytes.sublist(0, headersEndIndex));
    final responseHeadersMap =
        await _parseResponseHeaders(responseHeadersString);
    final statusCode = await _extractStatusCode(responseHeadersString);

    // Extract response body
    final responseBodyBytes = responseBytes.sublist(headersEndIndex);
    final responseStream =
        Stream.fromIterable([Uint8List.fromList(responseBodyBytes)]);

    // Return streamed response
    return http.StreamedResponse(
      responseStream,
      statusCode,
      contentLength: responseHeadersMap[CONTENT_LENGTH_KEY] != null
          ? int.tryParse(responseHeadersMap[CONTENT_LENGTH_KEY]!)
          : null,
      headers: responseHeadersMap,
    );
  }

  @override
  Future<void> close() async {
    for (var connectionKey in tlsConnections.keys.toList()) {
      final connection = tlsConnections[connectionKey];
      if (connection != null && !connection.isClosed()) {
        connection.close();
      }
      tlsConnections.remove(connectionKey);
    }
  }

  Future<List<int>> _readResponse(TlsConnection tlsConnection) async {
    List<int> concatenatedBytes = List<int>.empty(growable: true);
    int? headersEndIndex;

    // Read until the complete headers are found
    while (headersEndIndex == null) {
      List<int> readBytes = await tlsConnection.read(DEFAULT_READ_SIZE);
      if (readBytes.isEmpty) {
        throw Exception(
            "HttpClient: Connection closed before headers were complete.");
      }
      concatenatedBytes.addAll(readBytes);
      headersEndIndex = await _findHeadersEndIndex(concatenatedBytes);
    }

    final responseHeadersString =
        utf8.decode(concatenatedBytes.sublist(0, headersEndIndex));
    printInChunks(responseHeadersString);
    final responseHeadersMap =
        await _parseResponseHeaders(responseHeadersString);
    final contentLength = responseHeadersMap[CONTENT_LENGTH_KEY];
    final transferEncoding = responseHeadersMap[TRANSFER_ENCODING_KEY];

    List<int> responseBodyBytes;

    if (contentLength != null) {
      responseBodyBytes = await _readContentLengthResponse(tlsConnection,
          concatenatedBytes.sublist(headersEndIndex), int.parse(contentLength));
    } else if (transferEncoding == CHUNKED_ENCODING) {
      try {
        responseBodyBytes = await _readChunkedResponse(
            tlsConnection, concatenatedBytes.sublist(headersEndIndex));
      } catch (e) {
        throw Exception("HttpClient: Error on reading chunked data.");
      }
    } else {
      responseBodyBytes = await _readUntilClose(
          tlsConnection, concatenatedBytes.sublist(headersEndIndex));
    }

    return [...responseHeadersString.codeUnits, ...responseBodyBytes];
  }

  void printInChunks(String longString, {int chunkSize = 800}) {
    for (int i = 0; i < longString.length; i += chunkSize) {
      print(longString.substring(
          i,
          i + chunkSize > longString.length
              ? longString.length
              : i + chunkSize));
    }
  }

  Future<List<int>> _readChunkedResponse(
      TlsConnection tlsConnection, List<int> initialData) async {
    List<int> responseBodyBytes = [];
    String chunkSizeHex = '';
    bool inChunk = false;
    List<int> readData = initialData;

    int currentIndex = 0;
    while (currentIndex < readData.length) {
      // Read until the size of the next chunk is found
      final chunkEndIndex = _findChunkEndIndex(readData, currentIndex);
      // If the chunk size index does not exist, we need more data
      if (chunkEndIndex != null && chunkEndIndex != 0x0) {
        chunkSizeHex =
            utf8.decode(readData.sublist(currentIndex, chunkEndIndex)).trim();
        if (!RegExp(r'^[0-9a-fA-F]+$').hasMatch(chunkSizeHex)) {
          throw FormatException('Invalid chunk size format: "$chunkSizeHex".');
        }

        final chunkSize = int.parse(chunkSizeHex, radix: 16);
        currentIndex = chunkEndIndex + 2; // Move forward past the "CRLF"

        // If the chunk size is 0, it indicates the end of the response
        if (chunkSize == 0) {
          break;
        }

        // Read the chunk data (total size of initial data minus 6 bytes for "CRLF")
        int bytesAlreadyObtained = readData.length - 6;
        if (bytesAlreadyObtained <= chunkSize) {
          // Read more data from the connection
          final readBytes = await tlsConnection
              .read(DEFAULT_READ_SIZE)
              .timeout(Duration(seconds: 10));
          // Append the newly read data to readData
          readData.addAll(readBytes);
          currentIndex = 0;
        } else {
          final chunkData =
              readData.sublist(currentIndex, currentIndex + chunkSize);
          responseBodyBytes.addAll(chunkData);
          // Generate new readData excluding the processed chunk
          readData = readData.sublist(currentIndex + chunkSize);
          currentIndex = 0;
        }
      } else {
        // More data is needed
        final readBytes = await tlsConnection
            .read(DEFAULT_READ_SIZE)
            .timeout(Duration(seconds: 10));
        if (chunkEndIndex == 0x0) {
          // In this case, we are at the end of a chunk
          readData = readBytes;
        } else {
          // If it is null, more data is still missing
          readData.addAll(readBytes);
        }
      }
    }
    return responseBodyBytes;
  }

  Future<List<int>> _readUntilClose(
      TlsConnection tlsConnection, List<int> initialData) async {
    List<int> responseBodyBytes = [];
    // initialData contains the entire response lenght, we are ignoring it two bytes of size
    responseBodyBytes.addAll(initialData);

    while (true) {
      try {
        final readBytes = await tlsConnection
            .read(DEFAULT_READ_SIZE)
            .timeout(Duration(seconds: 10));

        if (readBytes.isEmpty) {
          break;
        }

        responseBodyBytes.addAll(readBytes);

        if (_containsEndDelimiter(responseBodyBytes)) {
          break;
        }
      } catch (e) {
        if (e is TimeoutException) {
          print('Timeout reached while reading response: $e');
          break;
        } else {
          rethrow;
        }
      }
    }

    return responseBodyBytes;
  }

  bool _containsEndDelimiter(List<int> data) {
    final endDelimiter = [0x0D, 0x0A];

    if (data.length >= endDelimiter.length) {
      for (int i = data.length - endDelimiter.length; i < data.length; i++) {
        if (data[i] != endDelimiter[i - (data.length - endDelimiter.length)]) {
          return false;
        }
      }
      return true;
    }

    return false;
  }

  int? _findChunkEndIndex(List<int> data, int startIndex) {
    // Buscar el final del tamaño de chunk, que debe terminar en "\r\n"
    for (int i = startIndex; i < data.length - 1; i++) {
      if (data[i] == 13 && data[i + 1] == 10) {
        // "\r\n"
        return i;
      }
    }
    return null;
  }

  Future<int?> _findHeadersEndIndex(List<int> bytes) async {
    const int CRLF = 0x0D;
    const int LF = 0x0A;

    for (int i = 0; i < bytes.length - 3; i++) {
      if (bytes[i] == CRLF &&
          bytes[i + 1] == LF &&
          bytes[i + 2] == CRLF &&
          bytes[i + 3] == LF) {
        return i + 4;
      }
    }
    return null;
  }

  String _constructRequestLine(String method, Uri uri) {
    return '${method} ${uri.path}${uri.query.isEmpty ? '' : '?${uri.query}'} ${HTTP_VERSION}';
  }

  StringBuffer _buildHeadersBuffer(Map<String, String> headers) {
    final buffer = StringBuffer();
    headers.forEach((name, value) {
      buffer.write('$name: $value$LINE_BREAK');
    });
    return buffer;
  }

  Future<List<int>> _readContentLengthResponse(TlsConnection tlsConnection,
      List<int> initialBytes, int contentLength) async {
    List<int> contentBytes = List<int>.from(initialBytes);
    int totalRead = contentBytes.length;

    while (totalRead < contentLength) {
      List<int> chunk = await tlsConnection.read(contentLength - totalRead);
      if (chunk.isEmpty) {
        throw Exception("HttpClient: Connection closed prematurely.");
      }
      contentBytes.addAll(chunk);
      totalRead = contentBytes.length;
    }

    return contentBytes;
  }

  Future<Map<String, String>> _parseResponseHeaders(
      String headersString) async {
    final headers = <String, String>{};
    final lines = headersString.split('\r\n');
    for (final line in lines) {
      final parts = line.split(': ');
      if (parts.length == 2) {
        headers[parts[0]] = parts[1];
      }
    }
    return headers;
  }

  Future<int> _extractStatusCode(String headersString) async {
    final parts = headersString.split(' ');
    if (parts.length >= 2) {
      return int.tryParse(parts[1]) ?? 200;
    }
    return 200;
  }
}
