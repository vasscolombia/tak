/// Identifies the device root status level detected.
enum RootStatus {
  /// Device is not rooted.
  NOT_ROOTED,

  /// Device may be rooted.
  MAYBE_ROOTED,

  /// Device is likely to be rooted.
  LIKELY_ROOTED,

  /// Device is rooted.
  ROOTED,
}
