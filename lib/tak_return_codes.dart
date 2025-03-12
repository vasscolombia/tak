///
/// Return codes we can return.
///
enum TakReturnCode {
  success,
  reRegisterSuccess,
  generalError,
  invalidParameter,
  invalidExecutionThread,
  invalidServerResponse,
  notAvailable,
  apiAlreadyInitialized,
  apiNotInitialized,
  deviceValidationFailed ,
  deviceUpdateFailed,
  deviceNoFingerprint,
  licenseExpired,
  licenseAboutToExpire,
  clientCertificateExpired,
  alreadyRegistered,
  notRegistered,
  securityCertificateError,
  storageNotFound,
  storageKeyNotFound,
  storageAlreadyExists,
  storageDeviceMismatch,
  networkTimeout,
  networkError,
  instanceWiped,
  instanceLocked
}

///
/// Mapper from return codes duplicated from T.A.K header to flutter enum.
///
class TakReturnCodeMapper {
  static const int _success = 0x00000000;
  static const int _reRegisterSuccess = 0x00000002;
  static const int _generalError = 0x000F0001;
  static const int _invalidParameter = 0x000F0003;
  static const int _invalidExecutionThread = 0x000F0004;
  static const int _invalidServerResponse = 0x000F0005;
  static const int _notAvailable = 0x000FFFFF;
  static const int _apiAlreadyInitialized = 0x00010001;
  static const int _apiNotInitialized = 0x00010002;
  static const int _deviceValidationFailed = 0x00020002;
  static const int _deviceUpdateFailed = 0x00020003;
  static const int _deviceNoFingerprint = 0x00020005;
  static const int _licenseExpired = 0x00020006;
  static const int _licenseAboutToExpire = 0x00020007;
  static const int _clientCertificateExpired = 0x00020008;
  static const int _alreadyRegistered = 0x00020009;
  static const int _notRegistered = 0x0002000A;
  static const int _securityCertificateError = 0x00030006;
  static const int _storageNotFound = 0x00050002;
  static const int _storageKeyNotFound = 0x00050003;
  static const int _storageAlreadyExists = 0x00050004;
  static const int _storageDeviceMismatch = 0x00050005;
  static const int _networkTimeout = 0x00030008;
  static const int _networkError = 0x00030009;
  static const int _instanceWiped = 0x0002000B;
  static const int _instanceLocked = 0x0002000C;

  static TakReturnCode mapErrorCode(int value) {
    switch (value) {
      case _success:
        return TakReturnCode.success;
      case _reRegisterSuccess:
        return TakReturnCode.reRegisterSuccess;
      case _invalidParameter:
        return TakReturnCode.invalidParameter;
      case _invalidServerResponse:
        return TakReturnCode.invalidServerResponse;
      case _notAvailable:
        return TakReturnCode.notAvailable;
      case _apiAlreadyInitialized:
        return TakReturnCode.apiAlreadyInitialized;
      case _apiNotInitialized:
        return TakReturnCode.apiNotInitialized;
      case _deviceValidationFailed:
        return TakReturnCode.deviceValidationFailed;
      case _deviceUpdateFailed:
        return TakReturnCode.deviceUpdateFailed;
      case _deviceNoFingerprint:
        return TakReturnCode.deviceNoFingerprint;
      case _licenseExpired:
        return TakReturnCode.licenseExpired;
      case _licenseAboutToExpire:
        return TakReturnCode.licenseAboutToExpire;
      case _clientCertificateExpired:
        return TakReturnCode.clientCertificateExpired;
      case _alreadyRegistered:
        return TakReturnCode.alreadyRegistered;
      case _notRegistered:
        return TakReturnCode.notRegistered;
      case _securityCertificateError:
        return TakReturnCode.securityCertificateError;
      case _storageNotFound:
        return TakReturnCode.storageNotFound;
      case _storageKeyNotFound:
        return TakReturnCode.storageKeyNotFound;
      case _storageAlreadyExists:
        return TakReturnCode.storageAlreadyExists;
      case _storageDeviceMismatch:
        return TakReturnCode.storageDeviceMismatch;
      case _networkError:
        return TakReturnCode.networkError;
      case _networkTimeout:
        return TakReturnCode.networkTimeout;
      case _instanceLocked:
        return TakReturnCode.instanceLocked;
      case _instanceWiped:
        return TakReturnCode.instanceWiped;
      case _invalidExecutionThread:
        return TakReturnCode.invalidExecutionThread;
      case _generalError:
      default:
        return TakReturnCode.generalError;
    }
  }
}

// Plugin exception mapping tak return codes

class TakException implements Exception {
  final TakReturnCode code;

  TakException(this.code);

  @override
  String toString() => 'TakException: $code';
}
