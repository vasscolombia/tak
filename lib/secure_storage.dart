import 'dart:convert';
import 'dart:ffi';
import 'dart:typed_data';

import 'package:ffi/ffi.dart';
import 'package:tak/native_tak/tak.dart';
import 'package:tak/native_tak/tak_byte_array_response.dart';
import 'package:tak/tak_plugin.dart';
import 'package:tak/tak_return_codes.dart';

// Use this class to interact with T.A.K's Secure Storage.
// Use [TakPlugin.getSecureStorage] to create an instance of this class.
class SecureStorage {
  final TakPlugin takPlugin;
  final String storageName;

  // Creates a new instance of the `SecureStorage` class
  SecureStorage(this.storageName, this.takPlugin) {
    _create();
  }

  // Creates a Secure Storage with the given name, encrypted with the available symmetric key.
  //
  // Throws TakException
  //   - [TakReturnCode.apiNotInitialized]          When library is not initialized.
  //   - [TakReturnCode.storageAlreadyExists]  A storage with this name already exists.
  //   - [TakReturnCode.invalidParameter]       Storage name is not valid
  void _create() {
    if (storageName.isEmpty) {
      throw TakException(TakReturnCode.invalidParameter);
    }
    final response =
        nativeCreateSecureStorage(storageName.toNativeUtf8().cast<Char>());
    TakReturnCode mapResponse = TakReturnCodeMapper.mapErrorCode(response);
    if (mapResponse != TakReturnCode.success &&
        mapResponse != TakReturnCode.storageAlreadyExists) {
      throw TakException(mapResponse);
    }
  }

  // Deletes this Secure Storage.
  //
  // Note:
  // - All data stored in this Secure Storage will be erased.
  // - This action cannot be undone!
  //
  // Throws TakException
  //   - [TakReturnCode.apiNotInitialized]          when library is not initialized.
  //   - [TakReturnCode.storageNotFound]       when storage object by the name provided does not exist.
  //   - [TakReturnCode.generalError]            when an unexpected error happens.
  void delete() {
    if (storageName.isEmpty) {
      throw TakException(TakReturnCode.invalidParameter);
    }
    final response =
        nativeDeleteSecureStorage(storageName.toNativeUtf8().cast<Char>());
    TakReturnCode mapResponse = TakReturnCodeMapper.mapErrorCode(response);
    if (mapResponse != TakReturnCode.success) {
      throw TakException(mapResponse);
    }
  }

  // Writes a key-value pair to the Secure Storage.
  //
  // If the key already exists, the value will be overwritten.
  //
  // Throws TakException
  //   - [TakReturnCode.apiNotInitialized]          when library is not initialized.
  //   - [TakReturnCode.invalidParameter]       when an input parameter is invalid.
  //   - [TakReturnCode.storageNotFound]       when storage object by the name provided does not exist.
  //   - [TakReturnCode.storageDeviceMismatch] when app is found to be running on a different device. In that case, storage is deleted for security reasons.
  //   - [TakReturnCode.generalError]            when an unexpected error happens.
  Future<void> write(String key, dynamic value) async {
    if (storageName.isEmpty || key.isEmpty) {
      throw TakException(TakReturnCode.invalidParameter);
    }
    Uint8List byteArray;
    if (value is int) {
      byteArray = _intToUint8List(value);
    } else if (value is String) {
      byteArray = Uint8List.fromList(utf8.encode(value));
    } else if (value is bool) {
      byteArray = Uint8List(1)..[0] = value ? 1 : 0;
    } else if (value is Uint8List) {
      byteArray = value;
    } else {
      throw TakException(TakReturnCode.invalidParameter);
    }
    final Pointer<Uint8> frameData = calloc.allocate<Uint8>(byteArray.length);
    final pointerList = frameData.asTypedList(byteArray.length);
    pointerList.setAll(0, byteArray);
    final response = nativeWriteSecureStorage(
        storageName.toNativeUtf8().cast<Char>(),
        key.toNativeUtf8().cast<Char>(),
        frameData.cast(),
        byteArray.length);
    TakReturnCode mapResponse = TakReturnCodeMapper.mapErrorCode(response);
    if (mapResponse != TakReturnCode.success) {
      throw TakException(mapResponse);
    }
  }

  // Reads a value from the Secure Storage.
  //
  // Parameters:
  // - key: The key under which the value was stored in the Secure Storage.
  //
  // Throws TakException
  //   - [TakReturnCode.apiNotInitialized]          when library is not initialized.
  //   - [TakReturnCode.storageNotFound]       when storage object by the name provided does not exist.
  //   - [TakReturnCode.storageKeyNotFound]    when storage object by this name does not exist.
  //   - [TakReturnCode.storageDeviceMismatch] when app is found to be running on a different device. In that case, storage is deleted for security reasons.
  //   - [TakReturnCode.generalError]            when an unexpected error happens.
  Uint8List read(String key) {
    if (storageName.isEmpty) {
      throw TakException(TakReturnCode.invalidParameter);
    }
    TakByteBufferResponse response = nativeReadSecureStorage(
        storageName.toNativeUtf8().cast<Char>(),
        key.toNativeUtf8().cast<Char>());
    TakReturnCode mapResponse =
        TakReturnCodeMapper.mapErrorCode(response.returnValue);
    if (mapResponse != TakReturnCode.success) {
      throw TakException(mapResponse);
    }

    Uint8List byteArray = response.getValue();
    return byteArray;
  }

  // Reads a string from the Secure Storage.
  //
  // Parameters:
  // - key: The key under which the value was stored in the Secure Storage.
  //
  // Throws TakException
  //   - [TakReturnCode.apiNotInitialized]          when library is not initialized.
  //   - [TakReturnCode.storageKeyNotFound]    when storage object by this name does not exist.
  //   - [TakReturnCode.storageNotFound]       when storage object by the name provided does not exist.
  //   - [TakReturnCode.storageDeviceMismatch] when app is found to be running on a different device. In that case, storage is deleted for security reasons.
  //   - [TakReturnCode.generalError]            when an unexpected error happens.
  String readString(String key) {
    Uint8List byteArray = read(key);
    return String.fromCharCodes(byteArray);
  }

  // Reads an integer value from the Secure Storage.
  //
  // Parameters:
  // - key: The key under which the value was stored in the Secure Storage.
  //
  // Throws TakException
  //   - [TakReturnCode.apiNotInitialized]          when library is not initialized.
  //   - [TakReturnCode.storageNotFound]       when storage object by the name provided does not exist.
  //   - [TakReturnCode.invalidParameter]       when call read Int but stored value is not an Int.
  //   - [TakReturnCode.storageKeyNotFound]    when storage object by this name does not exist.
  //   - [TakReturnCode.storageDeviceMismatch] when app is found to be running on a different device. In that case, storage is deleted for security reasons.
  //   - [TakReturnCode.generalError]            when an unexpected error happens.
  int readInt(String key) {
    Uint8List byteArray = read(key);
    ByteData byteData = ByteData.sublistView(byteArray);
    int integer = byteData.getUint32(0, Endian.big);
    return integer;
  }

  // Reads a boolean value from the Secure Storage.
  //
  // Parameters:
  // - key: The key under which the value was stored in the Secure Storage.
  //
  // Throws TakException
  //   - [TakReturnCode.apiNotInitialized]          when library is not initialized.
  //   - [TakReturnCode.invalidParameter]       when call read Bool but stored value is not a bool.
  //   - [TakReturnCode.storageKeyNotFound]    when storage object by this name does not exist.
  //   - [TakReturnCode.storageNotFound]       when storage object by the name provided does not exist.
  //   - [TakReturnCode.storageDeviceMismatch] when app is found to be running on a different device. In that case, storage is deleted for security reasons.
  //   - [TakReturnCode.generalError]            when an unexpected error happens.
  bool readBool(String key) {
    Uint8List byteArray = read(key);
    bool booleanRet = _uint8ListToBool(byteArray);
    return booleanRet;
  }

  // Deletes a key-value pair from the Secure Storage.
  //
  // Parameters:
  // - key: The key under which the value was stored in the Secure Storage.
  //
  // Throws TakException
  //   - [TakReturnCode.apiNotInitialized]          when library is not initialized.
  //   - [TakReturnCode.storageKeyNotFound]    when storage object by this name does not exist.
  //   - [TakReturnCode.storageNotFound]       when storage object by the name provided does not exist.
  //   - [TakReturnCode.storageDeviceMismatch] when app is found to be running on a different device. In that case, storage is deleted for security reasons.
  //   - [TakReturnCode.generalError]            when an unexpected error happens.
  void deleteEntry(String key) {
    if (storageName.isEmpty) {
      throw TakException(TakReturnCode.invalidParameter);
    }
    final response = nativeStorageDeleteEntry(
        storageName.toNativeUtf8().cast<Char>(),
        key.toNativeUtf8().cast<Char>());
    TakReturnCode mapResponse = TakReturnCodeMapper.mapErrorCode(response);
    if (mapResponse != TakReturnCode.success) {
      throw TakException(mapResponse);
    }
  }

  // Converts an integer to a Uint8List.
  //
  // Parameters:
  // - value: The integer value to be converted.
  //
  // Returns:
  // A Uint8List representing the integer value.
  //
  // Parameters:
  // - value: The integer value to be converted.
  //
  // Returns:
  // A Uint8List representing the integer value.
  Uint8List _intToUint8List(int value) {
    final byteData = ByteData(8);
    byteData.setInt32(0, value, Endian.big);
    return byteData.buffer.asUint8List();
  }

  // Converts a Uint8List to a boolean value.
  //
  // Parameters:
  // - byteArray: The Uint8List to be converted.
  //
  // Returns:
  // A boolean value representing the byteArray.
  //
  // Throws TakException
  //   - [TakReturnCode.invalidParameter]       when the byteArray is empty.
  bool _uint8ListToBool(Uint8List byteArray) {
    if (byteArray.isEmpty) {
      throw TakException(TakReturnCode.invalidParameter);
    }
    return byteArray[0] != 0;
  }
}
