/// Contains the result of a [register()] operation.
class RegisterResponse {
  /// The T.A.K identifier assigned to this device, in version 4 UUID format.
  final String takId;

  /// Indicates if the license is about to expire.
  final bool isLicenseAboutToExpire;

  /// Creates a new instance of the [RegisterResponse] class.
  RegisterResponse({required this.takId, required this.isLicenseAboutToExpire}); // TODO: can this constructor be internal?
}
