// ignore_for_file: always_specify_types
// ignore_for_file: camel_case_types
// ignore_for_file: non_constant_identifier_names

// AUTO GENERATED FILE, DO NOT EDIT.
//
// Generated by `package:ffigen`.
import 'dart:ffi' as ffi;

import 'package:ffi/ffi.dart';

import 'package:tak/native_tak/is_registered_response.dart';
import 'package:tak/native_tak/tak_byte_array_response.dart';
import 'package:tak/native_tak/tak_byte_buffer.dart';
import 'package:tak/native_tak/tak_id_response.dart';
import 'package:tak/tls/tls_connection_response.dart';

/// Bindings for `src/native_tak.h`.
///
/// Regenerate bindings with `flutter pub run ffigen --config ffigen.yaml`.
///
class TakBindings {
  /// Holds the symbol lookup function.
  final ffi.Pointer<T> Function<T extends ffi.NativeType>(String symbolName)
      _lookup;

  /// The symbols are looked up in [dynamicLibrary].
  TakBindings(ffi.DynamicLibrary dynamicLibrary)
      : _lookup = dynamicLibrary.lookup;

  /// The symbols are looked up with [lookup].
  TakBindings.fromLookup(
      ffi.Pointer<T> Function<T extends ffi.NativeType>(String symbolName)
          lookup)
      : _lookup = lookup;

  int native_initialize(
    ffi.Pointer<Utf8> path,
    ffi.Pointer<Utf8> license,
  ) {
    return _native_initialize(
      path,
      license,
    );
  }

  late final _native_initializePtr = _lookup<
      ffi.NativeFunction<
          ffi.Int32 Function(
              ffi.Pointer<Utf8>, ffi.Pointer<Utf8>)>>('native_initialize');
  late final _native_initialize = _native_initializePtr
      .asFunction<int Function(ffi.Pointer<Utf8>, ffi.Pointer<Utf8>)>();

  void native_release() {
    return _native_release();
  }

  late final _native_releasePtr =
      _lookup<ffi.NativeFunction<ffi.Void Function()>>('native_release');
  late final _native_release = _native_releasePtr.asFunction<void Function()>();

  int native_reset() {
    return _native_reset();
  }

  late final _native_resetPtr =
      _lookup<ffi.NativeFunction<ffi.Int32 Function()>>('native_reset');
  late final _native_reset = _native_resetPtr.asFunction<int Function()>();

  int native_register(ffi.Pointer<ffi.Char>? user) {
    return _native_register(user);
  }

  late final _native_registerPtr =
      _lookup<ffi.NativeFunction<ffi.Int32 Function(ffi.Pointer<ffi.Char>?)>>(
          'native_register');
  late final _native_register =
      _native_registerPtr.asFunction<int Function(ffi.Pointer<ffi.Char>?)>();

  IsRegisteredResponse native_isRegistered() {
    return _native_isRegistered();
  }

  late final _native_isRegisteredPtr =
      _lookup<ffi.NativeFunction<IsRegisteredResponse Function()>>(
          'native_isRegistered');
  late final _native_isRegistered =
      _native_isRegisteredPtr.asFunction<IsRegisteredResponse Function()>();

  bool native_isInitialized() {
    return _native_isInitialized();
  }

  late final _native_isInitializedPtr =
      _lookup<ffi.NativeFunction<ffi.Bool Function()>>('native_isInitialized');
  late final _native_isInitialized =
      _native_isInitializedPtr.asFunction<bool Function()>();

  ffi.Pointer<Utf8> native_getBuildVersion() {
    return _native_getBuildVersion();
  }

  late final _native_getBuildVersionPtr =
      _lookup<ffi.NativeFunction<ffi.Pointer<Utf8> Function()>>(
          'native_getBuildVersion');
  late final _native_getBuildVersion =
      _native_getBuildVersionPtr.asFunction<ffi.Pointer<Utf8> Function()>();

  ffi.Pointer<Utf8> native_getTakVersion() {
    return _native_getTakVersion();
  }

  late final _native_getTakVersionPtr =
      _lookup<ffi.NativeFunction<ffi.Pointer<Utf8> Function()>>(
          'native_getTakVersion');
  late final _native_getTakVersion =
      _native_getTakVersionPtr.asFunction<ffi.Pointer<Utf8> Function()>();

  int native_checkIntegrity() {
    return _native_checkIntegrity();
  }

  late final _native_checkIntegrityPtr =
      _lookup<ffi.NativeFunction<ffi.Int32 Function()>>(
          'native_checkIntegrity');
  late final _native_checkIntegrity =
      _native_checkIntegrityPtr.asFunction<int Function()>();

  TakIdResponse native_getTakId() {
    return _native_getTakId();
  }

  late final _native_getTakIdPtr =
      _lookup<ffi.NativeFunction<TakIdResponse Function()>>('native_getTakId');
  late final _native_getTakId =
      _native_getTakIdPtr.asFunction<TakIdResponse Function()>();

  bool native_getRootStatus() {
    return _native_getRootStatus();
  }

  late final _native_getRootStatusPtr =
      _lookup<ffi.NativeFunction<ffi.Bool Function()>>('native_getRootStatus');
  late final _native_getRootStatus =
      _native_getRootStatusPtr.asFunction<bool Function()>();

  int native_getAdvancedRootStatus() {
    return _native_getAdvancedRootStatus();
  }

  late final _native_getAdvancedRootStatusPtr =
      _lookup<ffi.NativeFunction<ffi.Int32 Function()>>(
          'native_getAdvancedRootStatus');
  late final _native_getAdvancedRootStatus =
      _native_getAdvancedRootStatusPtr.asFunction<int Function()>();

  int native_createRuntimeCheckThread(int timeInterval) {
    return _native_createRuntimeCheckThread(timeInterval);
  }

  late final _native_createRuntimeCheckThreadPtr =
      _lookup<ffi.NativeFunction<ffi.Int Function(ffi.Int)>>(
          'native_createRuntimeCheckThread');
  late final _native_createRuntimeCheckThread =
      _native_createRuntimeCheckThreadPtr.asFunction<int Function(int)>();

  bool native_isRuntimeThreadActive(bool relaunch) {
    return _native_isRuntimeThreadActive(relaunch);
  }

  late final _native_isRuntimeThreadActivePtr =
      _lookup<ffi.NativeFunction<ffi.Bool Function(ffi.Bool)>>(
          'native_isRuntimeThreadActive');
  late final _native_isRuntimeThreadActive =
      _native_isRuntimeThreadActivePtr.asFunction<bool Function(bool)>();

  int native_stopRuntimeThread() {
    return _native_stopRuntimeThread();
  }

  late final _native_stopRuntimeThreadPtr =
      _lookup<ffi.NativeFunction<ffi.Int32 Function()>>(
          'native_stopRuntimeThread');
  late final _native_stopRuntimeThread =
      _native_stopRuntimeThreadPtr.asFunction<int Function()>();

  TakByteBufferResponse native_fileProtectorDecryptFromFile(
      ffi.Pointer<ffi.Char> fileName, ffi.Pointer<ffi.Char> extension) {
    return _native_fileProtectorDecryptFromFile(fileName, extension);
  }

  late final _native_fileProtectorDecryptFromFilePtr = _lookup<
      ffi.NativeFunction<
          TakByteBufferResponse Function(ffi.Pointer<ffi.Char>,
              ffi.Pointer<ffi.Char>)>>('native_fileProtectorDecryptFromFile');
  late final _native_fileProtectorDecryptFromFile =
      _native_fileProtectorDecryptFromFilePtr.asFunction<
          TakByteBufferResponse Function(
              ffi.Pointer<ffi.Char>, ffi.Pointer<ffi.Char>)>();

  TakByteBufferResponse native_fileProtectorEncrypt(TakByteBuffer data) {
    return _native_fileProtectorEncrypt(data);
  }

  late final _native_fileProtectorEncryptPtr = _lookup<
          ffi.NativeFunction<TakByteBufferResponse Function(TakByteBuffer)>>(
      'native_fileProtectorEncrypt');
  late final _native_fileProtectorEncrypt = _native_fileProtectorEncryptPtr
      .asFunction<TakByteBufferResponse Function(TakByteBuffer)>();

  TakByteBufferResponse native_fileProtectorDecrypt(TakByteBuffer data) {
    return _native_fileProtectorDecrypt(data);
  }

  late final _native_fileProtectorDecryptPtr = _lookup<
          ffi.NativeFunction<TakByteBufferResponse Function(TakByteBuffer)>>(
      'native_fileProtectorDecrypt');
  late final _native_fileProtectorDecrypt = _native_fileProtectorDecryptPtr
      .asFunction<TakByteBufferResponse Function(TakByteBuffer)>();

  int native_createSecureStorage(ffi.Pointer<ffi.Char> storageName) {
    return _native_createSecureStorage(storageName);
  }

  late final _native_createSecureStoragePtr =
      _lookup<ffi.NativeFunction<ffi.Int32 Function(ffi.Pointer<ffi.Char>)>>(
          'native_storageCreate');
  late final _native_createSecureStorage = _native_createSecureStoragePtr
      .asFunction<int Function(ffi.Pointer<ffi.Char>)>();

  int native_deleteSecureStorage(ffi.Pointer<ffi.Char> storageName) {
    return _native_deleteSecureStorage(storageName);
  }

  late final _native_deleteSecureStoragePtr =
      _lookup<ffi.NativeFunction<ffi.Int32 Function(ffi.Pointer<ffi.Char>)>>(
          'native_storageDelete');
  late final _native_deleteSecureStorage = _native_deleteSecureStoragePtr
      .asFunction<int Function(ffi.Pointer<ffi.Char>)>();

  int native_writeStorage(ffi.Pointer<ffi.Char> storageName,
      ffi.Pointer<ffi.Char> key, ffi.Pointer<ffi.Char> value, int valueLength) {
    return _native_writeSecureStorage(storageName, key, value, valueLength);
  }

  late final _native_writeSecureStoragePtr = _lookup<
      ffi.NativeFunction<
          ffi.Int32 Function(ffi.Pointer<ffi.Char>, ffi.Pointer<ffi.Char>,
              ffi.Pointer<ffi.Char>, ffi.Int32)>>('native_storageWrite');
  late final _native_writeSecureStorage =
      _native_writeSecureStoragePtr.asFunction<
          int Function(ffi.Pointer<ffi.Char>, ffi.Pointer<ffi.Char>,
              ffi.Pointer<ffi.Char>, int)>();

  TakByteBufferResponse native_storageRead(
      ffi.Pointer<ffi.Char> storageName, ffi.Pointer<ffi.Char> key) {
    return _native_storageRead(storageName, key);
  }

  late final _native_storageReadPtr = _lookup<
      ffi.NativeFunction<
          TakByteBufferResponse Function(ffi.Pointer<ffi.Char>,
              ffi.Pointer<ffi.Char>)>>('native_storageRead');
  late final _native_storageRead = _native_storageReadPtr.asFunction<
      TakByteBufferResponse Function(
          ffi.Pointer<ffi.Char>, ffi.Pointer<ffi.Char>)>();

  int native_storageDeleteEntry(
      ffi.Pointer<ffi.Char> storageName, ffi.Pointer<ffi.Char> key) {
    return _native_storageDeleteEntry(storageName, key);
  }

  late final _native_storageDeleteEntryPtr = _lookup<
      ffi.NativeFunction<
          ffi.Int32 Function(ffi.Pointer<ffi.Char>,
              ffi.Pointer<ffi.Char>)>>('native_storageDeleteEntry');
  late final _native_storageDeleteEntry = _native_storageDeleteEntryPtr
      .asFunction<int Function(ffi.Pointer<ffi.Char>, ffi.Pointer<ffi.Char>)>();

  TlsConnectionResponse native_tlsConnectSecurePinning(
      ffi.Pointer<ffi.Char> fqdn, ffi.Pointer<ffi.Char> port, int timeout) {
    return _native_tlsConnectSecurePinning(fqdn, port, timeout);
  }

  late final _native_tlsConnectSecurePinningPtr = _lookup<
      ffi.NativeFunction<
          TlsConnectionResponse Function(
              ffi.Pointer<ffi.Char>,
              ffi.Pointer<ffi.Char>,
              ffi.Int32)>>('native_tlsConnectSecurePinning');
  late final _native_tlsConnectSecurePinning =
      _native_tlsConnectSecurePinningPtr.asFunction<
          TlsConnectionResponse Function(
              ffi.Pointer<ffi.Char>, ffi.Pointer<ffi.Char>, int)>();

  TakByteBufferResponse native_tlsReadAll(int socketDescriptor) {
    return _native_tlsReadAll(socketDescriptor);
  }

  late final _native_tlsReadAllPtr =
      _lookup<ffi.NativeFunction<TakByteBufferResponse Function(ffi.Int32)>>(
          'native_tlsReadAll');
  late final _native_tlsReadAll =
      _native_tlsReadAllPtr.asFunction<TakByteBufferResponse Function(int)>();

  TakByteBufferResponse native_tlsRead(int socketDescriptor, int max) {
    return _native_tlsRead(socketDescriptor, max);
  }

  late final _native_tlsReadPtr = _lookup<
      ffi.NativeFunction<
          TakByteBufferResponse Function(
              ffi.Int32, ffi.Int32)>>('native_tlsRead');
  late final _native_tlsRead =
      _native_tlsReadPtr.asFunction<TakByteBufferResponse Function(int, int)>();

  int native_tlsWrite(int socketDescriptor, ffi.Pointer<ffi.Char> data) {
    return _native_tlsWrite(socketDescriptor, data);
  }

  late final _native_tlsWritePtr = _lookup<
      ffi.NativeFunction<
          ffi.Int32 Function(
              ffi.Int32, ffi.Pointer<ffi.Char>)>>('native_tlsWrite');
  late final _native_tlsWrite = _native_tlsWritePtr
      .asFunction<int Function(int, ffi.Pointer<ffi.Char>)>();

  int native_tlsClose(int socketDescriptor) {
    return _native_tlsClose(socketDescriptor);
  }

  late final _native_tlsClosePtr =
      _lookup<ffi.NativeFunction<ffi.Int32 Function(ffi.Int32)>>(
          'native_tlsClose');
  late final _native_tlsClose =
      _native_tlsClosePtr.asFunction<int Function(int)>();

  bool native_tlsIsClosed(int socketDescriptor) {
    return _native_tlsIsClosed(socketDescriptor);
  }

  late final _native_tlsIsClosedPtr =
      _lookup<ffi.NativeFunction<ffi.Bool Function(ffi.Int32)>>(
          'native_tlsIsClosed');
  late final _native_tlsIsClosed =
      _native_tlsIsClosedPtr.asFunction<bool Function(int)>();

  TakByteBufferResponse native_getPinnedCertificates(
      ffi.Pointer<ffi.Char> hostName) {
    return _native_getPinnedCertificate(hostName);
  }

  late final _native_getPinnedCertificatePts = _lookup<
      ffi.NativeFunction<
          TakByteBufferResponse Function(
              ffi.Pointer<ffi.Char>)>>('native_getPinnedCertificate');
  late final _native_getPinnedCertificate = _native_getPinnedCertificatePts
      .asFunction<TakByteBufferResponse Function(ffi.Pointer<ffi.Char>)>();

  int native_updatePinnedCertificates() {
    return _native_updatePinnedCertificates();
  }

  late final _native_updatePinnedCertificatesPtr =
      _lookup<ffi.NativeFunction<ffi.Int32 Function()>>(
          'native_updatePinnedCertificates');
  late final _native_updatePinnedCertificates =
      _native_updatePinnedCertificatesPtr.asFunction<int Function()>();
}
