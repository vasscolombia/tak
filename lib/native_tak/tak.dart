import 'dart:ffi';
import 'dart:io';

import 'package:ffi/ffi.dart';

import 'package:tak/native_tak/is_registered_response.dart';
import 'package:tak/native_tak/tak_bindings_generated.dart';
import 'package:tak/native_tak/tak_byte_array_response.dart';
import 'package:tak/native_tak/tak_byte_buffer.dart';
import 'package:tak/native_tak/tak_id_response.dart';

/// A very short-lived native function.
///
/// For very short-lived functions, it is fine to call them on the main isolate.
/// They will block the Dart execution while running the native function, so
/// only do this for native functions which are guaranteed to be short-lived.
int nativeInitialize(
        Pointer<Utf8> workingPath, Pointer<Utf8> licenseFilePath) =>
    _bindings.native_initialize(workingPath, licenseFilePath);

Pointer<Utf8> nativeGetBuildVersion() => _bindings.native_getBuildVersion();

Pointer<Utf8> nativeGetTakVersion() => _bindings.native_getTakVersion();

void nativeRelease() => _bindings.native_release();

bool nativeIsInitialized() => _bindings.native_isInitialized();

int nativeRegister(Pointer<Char>? user) => _bindings.native_register(user);

int nativeCheckIntegrity() => _bindings.native_checkIntegrity();

IsRegisteredResponse nativeIsRegistered() => _bindings.native_isRegistered();

int nativeReset() => _bindings.native_reset();

TakIdResponse nativeGetTakId() => _bindings.native_getTakId();

bool nativeGetRootStatus() => _bindings.native_getRootStatus();

int nativeGetAdvancedRootStatus() => _bindings.native_getAdvancedRootStatus();

int nativeCreateRuntimeCheck(int timeInterval) =>
    _bindings.native_createRuntimeCheckThread(timeInterval);

bool nativeIsRuntimeThreadActive(bool relaunch) =>
    _bindings.native_isRuntimeThreadActive(relaunch);

int nativeStopRuntimeThread() => _bindings.native_stopRuntimeThread();

TakByteBufferResponse nativeFileProtectorDecryptFromFile(
        Pointer<Char> fileName, Pointer<Char> extension) =>
    _bindings.native_fileProtectorDecryptFromFile(fileName, extension);

TakByteBufferResponse nativeFileProtectorEncrypt(TakByteBuffer data) =>
    _bindings.native_fileProtectorEncrypt(data);

TakByteBufferResponse nativeFileProtectorDecrypt(TakByteBuffer data) =>
    _bindings.native_fileProtectorDecrypt(data);

int nativeCreateSecureStorage(Pointer<Char> storageName) =>
    _bindings.native_createSecureStorage(storageName);

int nativeDeleteSecureStorage(Pointer<Char> storageName) =>
    _bindings.native_deleteSecureStorage(storageName);

int nativeWriteSecureStorage(Pointer<Char> storageName, Pointer<Char> key,
        Pointer<Char> value, int valueLength) =>
    _bindings.native_writeStorage(storageName, key, value, valueLength);

TakByteBufferResponse nativeReadSecureStorage(
        Pointer<Char> storageName, Pointer<Char> key) =>
    _bindings.native_storageRead(storageName, key);

int nativeStorageDeleteEntry(Pointer<Char> storageName, Pointer<Char> key) =>
    _bindings.native_storageDeleteEntry(storageName, key);

const String _libName = 'tak_flutter_wrapper';

/// The dynamic library in which the symbols for [TakBindings] can be found.
final DynamicLibrary _dylib = () {
  if (Platform.isMacOS || Platform.isIOS) {
    return DynamicLibrary.executable();
  }
  if (Platform.isAndroid || Platform.isLinux) {
    return DynamicLibrary.open('lib$_libName.so');
  }
  if (Platform.isWindows) {
    return DynamicLibrary.open('$_libName.dll');
  }
  throw UnsupportedError('Unknown platform: ${Platform.operatingSystem}');
}();

/// The bindings to the native functions in [_dylib].
final TakBindings _bindings = TakBindings(_dylib);
