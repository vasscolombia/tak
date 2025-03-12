import 'dart:ffi';
import 'package:ffi/ffi.dart';

final class TakIdResponse extends Struct {
  /// A pointer to a UTF-8 encoded string representing the TAK identifier.
  external Pointer<Utf8> takId;

  /// An unsigned integer field representing the return code or status of the operation.
  @Int32()
  external int returnCode;
}
