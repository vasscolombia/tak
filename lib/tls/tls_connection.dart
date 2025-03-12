import 'dart:ffi';
import 'dart:typed_data';

import 'package:ffi/ffi.dart';
import 'package:tak/native_tak/tak.dart';
import 'package:tak/native_tak/tak_byte_array_response.dart';
import 'package:tak/tak_return_codes.dart';
import 'package:tak/tls/tls_connection_response.dart';

/// A class representing a TLS connection.
///
/// This class provides methods to establish a TLS connection with secure pinning, write data to, read data from,
/// check if the connection is closed, and close the connection.
///
class TlsConnection {
  final String fqdn;
  final String port;
  final int timeout;
  late final int socketDescriptor;

  /// Creates a new instance of [TlsConnection].
  ///
  /// [fqdn]: Fully Qualified Domain Name of the server.
  /// [port]: Port number for the connection.
  /// [timeout]: Timeout for the connection in milliseconds.
  ///
  /// Throws:
  ///   - [TakException] with the relevant [TakReturnCode] if the connection fails.
  TlsConnection(
      {required this.fqdn, required this.port, required this.timeout}) {
    _connect();
  }

  void _connect() {
    TlsConnectionResponse response = nativeTlsConnectSecurePinning(
        fqdn.toNativeUtf8().cast<Char>(),
        port.toNativeUtf8().cast<Char>(),
        timeout);
    TakReturnCode mapResponse =
        TakReturnCodeMapper.mapErrorCode(response.returnCode);

    if (mapResponse == TakReturnCode.success) {
      socketDescriptor = response.socketDescriptor;
    } else {
      throw TakException(mapResponse);
    }
  }

  /// Writes data to the TLS connection.
  ///
  /// [data]: The data to write to the connection.
  ///
  /// Throws:
  ///   - [TakException] with the relevant [TakReturnCode] if the write operation fails.
  Future<void> write(Pointer<Char> data) async {
    int response = nativeTlsWrite(socketDescriptor, data);
    TakReturnCode mapResponse = TakReturnCodeMapper.mapErrorCode(response);

    if (mapResponse != TakReturnCode.success) {
      throw TakException(mapResponse);
    }
  }

  /// Reads data from the TLS connection.
  ///
  /// [max]: The maximum amount of data to read.
  ///
  /// Returns:
  ///   - A [Uint8List] containing the data read from the connection.
  ///
  /// Throws:
  ///   - [TakException] with the relevant [TakReturnCode] if the read operation fails.
  Future<Uint8List> read(int max) async {
    TakByteBufferResponse response = nativeTlsRead(socketDescriptor, max);

    TakReturnCode mapResponse =
        TakReturnCodeMapper.mapErrorCode(response.returnValue);

    if (mapResponse != TakReturnCode.success) {
      throw TakException(mapResponse);
    }

    return response.getValue();
  }

  /// Checks if the TLS connection is closed.
  ///
  /// Returns:
  ///   - `true` if the connection is closed, `false` otherwise.
  bool isClosed() {
    bool response = nativeTlsIsClosed(socketDescriptor);
    return response;
  }

  /// Closes the TLS connection.
  ///
  /// Throws:
  ///   - [TakException] with the relevant [TakReturnCode] if the close operation fails.

  void close() {
    int response = nativeTlsClose(socketDescriptor);
    TakReturnCode mapResponse = TakReturnCodeMapper.mapErrorCode(response);
    if (mapResponse != TakReturnCode.success) {
      throw TakException(mapResponse);
    }
  }
}
