import 'dart:ffi';

final class IsRegisteredResponse extends Struct {
  /// An integer field representing the registration status.
  @Bool()
  external bool isRegistered;

  /// An integer field representing any error that occurred during the operation.
  @Int32()
  external int returnCode;
}
