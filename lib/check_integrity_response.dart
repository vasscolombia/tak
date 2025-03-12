/// Contains the result of a [checkIntegrity()] operation.
class CheckIntegrityResponse {
  /// The T.A.K identifier assigned to this device, in version 4 UUID format.
  final String takId;

  /// Indicates whether a re-registration occurred.
  final bool didReRegister;

  /// Indicates if the license is about to expire.
  final bool isLicenseAboutToExpire;

  /// Creates a new instance of the [CheckIntegrityResponse] class.
  CheckIntegrityResponse({
    required this.takId,
    required this.didReRegister,
    required this.isLicenseAboutToExpire,
  }); // TODO: can this constructor be internal?
}
