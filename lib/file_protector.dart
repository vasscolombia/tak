import 'dart:ffi';
import 'dart:typed_data';

import 'package:tak/native_tak/tak.dart';
import 'package:tak/native_tak/tak_byte_array_response.dart';
import 'package:tak/native_tak/tak_byte_buffer.dart';
import 'package:tak/tak_plugin.dart';
import 'package:tak/tak_return_codes.dart';

import 'package:ffi/ffi.dart';

/// Use this class to interact with the "file protection" API of T.A.K.
/// The `FileProtector` class can be used to protect (encrypt/decrypt) files or large data.
/// Use [TakPlugin.getFileProtector] to create an instance of this class.
class FileProtector {
  final TakPlugin takPlugin;
  FileProtector(this.takPlugin);

  /// Decrypts a byte buffer.
  ///
  /// The function decrypts the encrypted data from the specified file using the T.A.K file protector.
  ///
  /// [fileName]: The name of the file to be decrypted.
  ///
  /// Returns the decrypted data as a Uint8List.
  ///
  /// Throws a [TakException] with the following error codes:
  /// - [TakReturnCode.apiNotInitialized] when T.A.K was not initialized before calling this method.
  /// - [TakReturnCode.invalidParameter] when input data has length 0.
  /// - [TakReturnCode.generalError] when an unexpected error happens.
  Uint8List decryptFromFile(String fileName) {
    String file = 'flutter_assets/assets/$fileName';
    TakByteBufferResponse response = nativeFileProtectorDecryptFromFile(
        file.toNativeUtf8().cast<Char>(), "tak".toNativeUtf8().cast<Char>());
    TakReturnCode mapResponse =
        TakReturnCodeMapper.mapErrorCode(response.returnValue);
    if (mapResponse != TakReturnCode.success) {
      throw TakException(mapResponse);
    } else {
      return response.getValue();
    }
  }

  /// Encrypts a byte buffer, returning it ready to be securely stored in persistent storage.
  ///
  /// The data is encrypted with a random AES key in GCM mode. The random AES key gets then encrypted by the WBC
  /// customer key. The encrypted data together with the encrypted random AES key is stored in the output buffer.
  ///
  /// [plainData]: Data to be encrypted.
  ///
  /// Returns the encrypted data, ready to be securely stored in persistent storage.
  ///
  /// Throws a [TakException] with the following error codes:
  /// - [TakReturnCode.apiNotInitialized] when T.A.K was not initialized before calling this method.
  /// - [TakReturnCode.invalidParameter] when plain data has length 0.
  /// - [TakReturnCode.generalError] when an unexpected error happens.
  /// - [TakReturnCode.instanceLocked] when the application has been remotely locked.
  ///                                  This method is unavailable until the instance is unlocked.

  Uint8List encrypt(Uint8List dataToEncrypt) {
    final dataPointer = uint8ListToPointer(dataToEncrypt);

    final takByteBufferPtr = calloc<TakByteBuffer>();
    final takByteBuffer = takByteBufferPtr.ref
      ..buffer = dataPointer
      ..bufferLength = dataToEncrypt.length;
    TakByteBufferResponse response = nativeFileProtectorEncrypt(takByteBuffer);

    TakReturnCode mapResponse =
        TakReturnCodeMapper.mapErrorCode(response.returnValue);
    if (mapResponse != TakReturnCode.success) {
      throw TakException(mapResponse);
    } else {
      return response.getValue();
    }
  }

  /// Decrypts a byte buffer.
  ///
  /// The function first decrypts the encrypted random AES key with the customer WBC key.
  /// The decrypted random AES key is then used to decrypt the "actual" input data (payload).
  /// The decrypted data is then stored in the output buffer.
  ///
  /// [encryptedData]: Data to be decrypted.
  ///
  /// Returns the decrypted data as a byte array.
  ///
  /// Throws a [TakException] in case of errors, including:
  /// - [TakReturnCode.apiNotInitialized] when T.A.K was not initialized before calling this method.
  /// - [TakReturnCode.invalidParameter] when plain data has length 0.
  /// - [TakReturnCode.generalError] when an unexpected error happens.

  Uint8List decrypt(Uint8List dataToDecrypt) {
    final dataPointer = uint8ListToPointer(dataToDecrypt);

    final takByteBufferPtr = calloc<TakByteBuffer>();
    final takByteBuffer = takByteBufferPtr.ref
      ..buffer = dataPointer
      ..bufferLength = dataToDecrypt.length;
    TakByteBufferResponse response = nativeFileProtectorDecrypt(takByteBuffer);

    TakReturnCode mapResponse =
        TakReturnCodeMapper.mapErrorCode(response.returnValue);
    if (mapResponse != TakReturnCode.success) {
      throw TakException(mapResponse);
    } else {
      return response.getValue();
    }
  }

  /// Converts a Uint8List to a Pointer<Uint8>.
  ///
  /// This function allocates memory on the native heap using calloc,
  /// copies the bytes from the Uint8List to the allocated memory,
  /// and returns a Pointer to the allocated memory.
  /// Parameters: - list: The Uint8List to convert to a Pointer.
  /// Returns:  A Pointer<Uint8> pointing to the memory allocated on the native heap.
  Pointer<Uint8> uint8ListToPointer(Uint8List list) {
    final ptr = calloc<Uint8>(list.length);
    for (var i = 0; i < list.length; i++) {
      ptr[i] = list[i];
    }
    return ptr;
  }
}
