import 'dart:ffi';

import 'package:ffi/ffi.dart';

final class TlsConnectionResponse extends Struct {
  @Int32()
  external int socketDescriptor;

  external Pointer<Utf8> peerCertificate;

  @Int32()
  external int returnCode;
}
