import 'dart:ffi';
import 'dart:typed_data';

import 'package:tak/native_tak/tak_byte_buffer.dart';

// final class named TakByteBufferResponse, which extends Struct
// A Struct in Dart FFI represents a C struct
final class TakByteBufferResponse extends Struct {
  // Defining an integer field named returnValue using Dart FFI's @Int32() annotation
  @Int32()
  external int returnValue;

  // Defining a pointer field named buffer to hold a pointer and length to unsigned 8-bit integers (Uint8)
  external TakByteBuffer takByteBuffer;

  // A method named getValue() which returns a Uint8List from the buffer
  Uint8List getValue() {
    // Converting the buffer pointer to a Uint8List of given length
    return takByteBuffer.buffer.asTypedList(takByteBuffer.bufferLength);
  }
}
