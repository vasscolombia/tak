import 'dart:ffi';
import 'package:ffi/ffi.dart';
import 'package:flutter/services.dart';

import 'package:tak/check_integrity_response.dart';
import 'package:tak/file_protector.dart';
import 'package:tak/native_tak/is_registered_response.dart';
import 'package:tak/native_tak/tak_id_response.dart';
import 'package:tak/register_response.dart';
import 'package:tak/native_tak/tak.dart';
import 'package:tak/root_status_response.dart';
import 'package:tak/secure_storage.dart';
import 'package:tak/tak_return_codes.dart';
import 'package:tak/tls/tak_client_http.dart';

/// A class representing the T.A.K Plugin for interacting with the SDK.
///
/// This class provides static method for initialization and once initialized
/// all the methods to acces the SDK.
class TakPlugin {
  // Private constructor
  TakPlugin._();

  /// Initializes the T.A.K Plugin.
  ///
  /// This method creates a new instance of the TakPlugin class.
  ///
  /// Parameter license: Name (without the ".tak" extension) of the license file to be used for initializing the lib.
  ///
  static Future<TakPlugin> initialize(String license) async {
    const platform = MethodChannel('tak');
    String workingPath = await platform.invokeMethod('loadEnvironment');
    String licenseDirectory = "flutter_assets/assets/$license";
    Pointer<Utf8> workingDir = workingPath.toNativeUtf8();
    int response = nativeInitialize(workingDir, licenseDirectory.toNativeUtf8());
    TakReturnCode mapResponse = TakReturnCodeMapper.mapErrorCode(response);
    if (mapResponse == TakReturnCode.success) {
      // If initialization is successful: do nothing.
    } else if (mapResponse == TakReturnCode.apiAlreadyInitialized) {
      // Do nothing
    } else if (mapResponse == TakReturnCode.licenseAboutToExpire) {
      // Do nothing
    } else if (mapResponse == TakReturnCode.instanceLocked) {
      // Do nothing
    } else {
      // Logic to handle invalid params on tak initialize.
      throw TakException(mapResponse);
    }

    return TakPlugin._();
  }

  /// Returns the build version of the T.A.K-Client library.
  /// This is the version number transmitted to the T.A.K server on "register" and "validate" calls. Due to the design of
  /// T.A.K, this number changes for every T.A.K-Client build, even if there are no code changes.
  ///
  /// - Returns: the build version string of the T.A.K-Client library
  static Future<String> getTakBuildVersion() async {
    return nativeGetBuildVersion().toDartString();
  }

  /// Returns the software version of the T.A.K-Client library.
  /// This is the "official" T.A.K lib version, compliant with semantic versioning.
  ///
  /// Returns: the software version of the T.A.K-Client library.
  static Future<String> getTakVersion() async {
    return nativeGetTakVersion().toDartString();
  }

  /// Releases and disposes of the current SDK instance.
  /// Releases all the memory used by the T.A.K library.
  ///
  /// Remarks:
  ///   - This method needs to be called when the instance will no longer be used.
  ///   - Any threads still running in the background will be stopped.
  void dispose() {
    nativeRelease();
  }

  /// Verifies whether the SDK is initialized or not.
  ///
  /// This method verifies if the T.A.K SDK is initialized and ready to be used.
  ///
  /// Returns `true` if the SDK is initialized, `false` otherwise.
  bool isInitialized() {
    return nativeIsInitialized();
  }

  /// Registers the T.A.K library with the T.A.K server.
  ///
  /// [userHash]: Hexadecimal string uniquely identifying the end user, to be used for data privacy purposes.
  ///             If null is provided, the T.A.K library will assign a random user hash to the current device.
  ///
  /// Returns a [RegisterResponse] object containing the result of the operation.
  ///
  /// Throws [TakException] with the following error codes:
  /// - [TakReturnCode.apiNotInitialized] when the library is not initialized.
  /// - [TakReturnCode.invalidParameter] when `userHash` is invalid or empty.
  /// - [TakReturnCode.alreadyRegistered] when the library is already registered.
  /// - [TakReturnCode.licenseAboutToExpire] when the license is expired. T.A.K will not work unless the
  ///   app is updated with a new license.
  /// - [TakReturnCode.deviceNoFingerprint] when the device fingerprint could not be retrieved.
  /// - [TakReturnCode.notAvailable] when T.A.K is offline or the certificate has expired.
  /// - [TakReturnCode.invalidServerResponse] when the server response was not as expected.
  /// - [TakReturnCode.networkTimeout] when a network timeout problem occurs. Try again later.
  /// - [TakReturnCode.networkError] when a network error occurs. Try again later.
  /// - [TakReturnCode.generalError] when an unexpected error has happened.
  Future<RegisterResponse> register(String? userHash) async {
    Pointer<Char> userHashConvert = nullptr;
    if (userHash != null && userHash.isEmpty) {
      throw TakException(TakReturnCode.invalidParameter);
    }
    // Convert userHash to native UTF-8
    if (userHash != null) {
      userHashConvert = userHash.toNativeUtf8().cast<Char>();
    }
    // Call register function with converted userHash
    final response = nativeRegister(userHashConvert);
    TakReturnCode mapResponse = TakReturnCodeMapper.mapErrorCode(response);
    if (mapResponse != TakReturnCode.success &&
        mapResponse != TakReturnCode.licenseAboutToExpire) {
      // Throw exception for any error other than success and licenseAboutToExpire
      throw TakException(mapResponse);
    } else {
      // Return a RegisterResponse with default values
      return RegisterResponse(
        takId:
            getTakIdentifier(), // Assuming getTakIdentifier() retrieves the TAK identifier
        isLicenseAboutToExpire:
            mapResponse == TakReturnCode.licenseAboutToExpire,
      );
    }
  }

  /// Checks whether the T.A.K lib has already been registered with the T.A.K Server.
  ///
  /// Returns true if the T.A.K lib has already been registered, false otherwise.
  ///
  /// Throws a [TakException] with the following error code:
  /// - [TakReturnCode.apiNotInitialized] when the library is not initialized.
  bool isRegistered() {
    // Call isRegistered function to check registration status
    IsRegisteredResponse response = nativeIsRegistered();
    // Map the response to TakReturnCode
    TakReturnCode mapResponse =
        TakReturnCodeMapper.mapErrorCode(response.returnCode);
    // Check the response and return the appropriate result or throw an exception
    if (mapResponse == TakReturnCode.success &&
        response.isRegistered == false) {
      return false; // Not registered
    }
    if (mapResponse == TakReturnCode.success && response.isRegistered == true) {
      return true; // Registered
    } else {
      throw TakException(mapResponse); // Throw exception for other cases
    }
  }

  /// Syncs the device state with the T.A.K server. If the T.A.K client certificate is about to expire, a
  /// re-registration is performed.
  ///
  /// Note:
  /// - A re-registration will change the T.A.K identifier!
  ///
  /// Returns a [CheckIntegrityResponse] object containing the result of the operation.
  ///
  /// Throws [TakException] with the following error codes:
  /// - [TakReturnCode.apiNotInitialized] when the library is not initialized.
  /// - [TakReturnCode.notRegistered] when the library was not previously registered.
  /// - [TakReturnCode.licenseExpired] when the license is expired. T.A.K will not work unless the app
  ///   is updated with a new license.
  /// - [TakReturnCode.clientCertificateExpired] when the app needs to register again.
  /// - [TakReturnCode.networkTimeout] when a network timeout problem occurs. Try again later.
  /// - [TakReturnCode.networkError] when a network error occurs. Try again later.
  /// - [TakReturnCode.generalError when an unexpected error has happened.
  /// - [TakReturnCode.instanceWiped] when the application has been remotely wiped.
  /// - [TakReturnCode.instanceLocked] this is a warning, the application has been remotely locked.
  ///   Some of the functionalities will not be available until the instance is unlocked.
  Future<CheckIntegrityResponse> checkIntegrity() async {
    final response = nativeCheckIntegrity();
    TakReturnCode mapResponse = TakReturnCodeMapper.mapErrorCode(response);
    if (mapResponse != TakReturnCode.success &&
        mapResponse != TakReturnCode.licenseAboutToExpire &&
        mapResponse != TakReturnCode.reRegisterSuccess) {
      // It will throw exception for any error other than:
      // success, licenseAboutToExpire, reRegisterSuccess, and notRegistered.
      throw TakException(mapResponse);
    } else {
      // Return a CheckIntegrityResponse with default values
      return CheckIntegrityResponse(
        takId:
            getTakIdentifier(), // Assuming getTakIdentifier() retrieves the TAK identifier
        didReRegister: mapResponse == TakReturnCode.reRegisterSuccess,
        isLicenseAboutToExpire:
            mapResponse == TakReturnCode.licenseAboutToExpire,
      );
    }
  }

  /// Returns the T.A.K identifier assigned to the client library when it was registered.
  ///
  /// The T.A.K identifier has the format of a version 4 UUID.
  ///
  /// Returns the T.A.K identifier assigned to the device, in version 4 UUID format string.
  ///
  /// Throws [TakException] with the following error codes:
  /// - [TakReturnCode.apiNotInitialized] when the library is not initialized.
  /// - [TakReturnCode.notRegistered] when the library was not previously registered.
  /// - [TakReturnCode.generalError] when an unexpected error has happened.
  /// - [TakReturnCode.instanceLocked] when the application has been remotely locked.
  ///   This method is unavailable until the instance is unlocked.
  String getTakIdentifier() {
    TakIdResponse response = nativeGetTakId();
    TakReturnCode mapResponse =
        TakReturnCodeMapper.mapErrorCode(response.returnCode);
    if (mapResponse == TakReturnCode.success) {
      String responseToString = response.takId.toDartString();
      return responseToString;
    } else {
      throw TakException(mapResponse);
    }
  }

  /// Deletes all registration data stored by T.A.K. Use this method to "undo" a registration.
  ///
  /// Note:
  /// - All user data and keys stored by T.A.K will be deleted!
  ///
  /// Throws:
  /// - [TakException] with [TakReturnCode.apiNotInitialized] if T.A.K was not initialized before using this method.
  /// - [TakException] with [TakReturnCode.generalError] if an unexpected error happens.
  /// - [TakException] with [TakReturnCode.instanceLocked] if the application has been remotely locked.
  ///   This method is unavailable until the instance is unlocked.
  void reset() {
    int response = nativeReset();
    TakReturnCode mapResponse = TakReturnCodeMapper.mapErrorCode(response);
    if (mapResponse != TakReturnCode.success) {
      throw TakException(mapResponse);
    }
  }

  /// Checks if the device is rooted.
  ///
  /// Returns `true` if the device is rooted, `false` otherwise.
  /// In a remote locked status, it will always return `false`.
  bool isRooted() {
    return nativeGetRootStatus();
  }

  /// Checks the root status of the device.
  ///
  /// Returns a [RootStatus] enum. In a remote locked status, it will always return `RootStatus.NOT_ROOTED`.
  RootStatus getAdvancedRootStatus() {
    int advancedRootStatusEnumInt = nativeGetAdvancedRootStatus();
    return RootStatus.values[advancedRootStatusEnumInt];
  }

  /// Creates a background thread that launches runtime checks.
  ///
  /// This method creates a thread that will launch runtime checks periodically in the background.
  ///
  /// [timeInterval] is the interval in seconds to repeat the thread's execution. The minimum value is 0.
  /// The recommended amount of seconds is also 0 seconds,
  /// but if performance is key then it is recommended to increase it to 15 seconds.
  ///
  /// Throws a [TakException] in the following cases:
  ///   - [TakReturnCode.invalidParameter] when the amount of seconds is below the minimum 0 seconds.
  ///   - [TakReturnCode.apiNotInitialized] when T.A.K is not in an "initialized" status
  ///                                       (i.e. `tak.dispose()` has been called).
  ///   - [TakReturnCode.generalError] when an unexpected error happens.
  void createRuntimeCheckThread(int timeInterval) {
    int response = nativeCreateRuntimeCheck(timeInterval);
    TakReturnCode mapResponse = TakReturnCodeMapper.mapErrorCode(response);
    if (mapResponse != TakReturnCode.success) {
      throw TakException(mapResponse);
    }
  }

  /// Checks if the runtime checks' thread is running.
  ///
  /// This method will check if the runtime checks' thread is running. In case it should be running, additionally,
  /// it will relaunch the runtime checks' thread if [relaunch] is `true`.
  ///
  /// [relaunch] is a boolean value that indicates whether to relaunch the thread if it is found unexpectedly inactive.
  ///
  /// Returns `true` if the Runtime Thread is active, `false` otherwise.
  bool isRuntimeThreadActive(bool relaunch) {
    return nativeIsRuntimeThreadActive(relaunch);
  }

  /// Stops the runtime checks' thread if running.
  ///
  /// This method stops the thread that periodically executes the runtime checks if it is running.
  ///
  /// Throws a [TakException] with the following error codes:
  ///   - [ TakReturnCode.generalError ] : When an unexpected error happens.
  ///   - [ TakReturnCode.apiNotInitialized ] when T.A.K is not in an "initialized" status
  ///                                         (i.e. `tak.dispose()` has been called).
  void stopRuntimeThread() {
    int response = nativeStopRuntimeThread();
    TakReturnCode mapResponse = TakReturnCodeMapper.mapErrorCode(response);
    if (mapResponse != TakReturnCode.success) {
      throw TakException(mapResponse);
    }
  }

  /// Returns an instance of the [FileProtector] class.
  /// The `FileProtector` class can be used to protect (encrypt/decrypt) files or large data.
  /// Throws a [TakException] with [TakReturnCode.apiNotInitialized] if the T.A.K API is not initialized.
  /// Returns an instance of the [FileProtector] class.
  FileProtector getFileProtector() {
    if (!isInitialized()) {
      throw TakException(TakReturnCode.apiNotInitialized);
    }

    return FileProtector(this);
  }

  /// Opens the secure storage with the given name. If it does not exist, it will be created.
  ///
  /// Returns a [SecureStorage] object.
  ///
  /// Throws a [TakException] with the following error codes:
  /// - [TakReturnCode.apiNotInitialized] when the library is not initialized.
  /// - [TakReturnCode.deviceNoFingerprint] when the device fingerprint could not be retrieved.
  /// - [TakReturnCode.storageDeviceMismatch] when the app is found to be running on a different device.
  ///                                           In that case, storage is deleted for security reasons.
  /// - [TakReturnCode.invalidParameter] when the storage name is not valid.
  /// - [TakReturnCode.generalError] when an unexpected error happens.
  /// - [TakReturnCode.instanceLocked] when the application has been remotely locked.
  ///                                     This method is unavailable until the instance is unlocked.
  SecureStorage getSecureStorage(String storageName) {
    if (!isInitialized()) {
      throw TakException(TakReturnCode.apiNotInitialized);
    }

    return SecureStorage(storageName, this);
  }

  /// Creates a HTTP Client that is using the T.A.K Secure Channel internally.
  /// Certificate Pinning is enforced.
  /// 
  /// If host requests client authentication, the client private key and certificate will be used to authenticate.
  ///
  /// Returns a [TakHttpClient] object.
  ///
  /// Throws [TakException] with the following error codes:
  /// - [TakReturnCode.apiNotInitialized] when the library is not initialized.
  ///
  TakHttpClient getHttpClient() {
    if (!isInitialized()) {
      throw TakException(TakReturnCode.apiNotInitialized);
    }

    return TakHttpClient();
  }
}
