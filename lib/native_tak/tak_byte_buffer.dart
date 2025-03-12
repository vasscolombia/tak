import 'dart:ffi';
import 'package:ffi/ffi.dart';

sealed class TakByteBuffer extends Struct {
  external Pointer<Uint8> buffer;
  @Int32()
  external int bufferLength;

  factory TakByteBuffer.allocate(Pointer<Uint8> buffer, int bufferLength) =>
      calloc<TakByteBuffer>().ref
        ..buffer = buffer
        ..bufferLength = bufferLength;
}
