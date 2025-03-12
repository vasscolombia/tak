#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html.
# Run `pod lib lint tak.podspec` to validate before publishing.
#
Pod::Spec.new do |s|
  s.name             = 'tak'
  s.version          = '2.17.2'
  s.summary          = 'TAK Flutter FFI plugin project.'
  s.description      = <<-DESC
TAK Flutter FFI plugin project.
                       DESC
  s.homepage         = 'https://build38.com'
  s.license          = { :file => '../LICENSE' }
  s.author           = { 'Build38' => 'info@example.com' }

  # This will ensure the source files in Classes/ are included in the native
  # builds of apps using this FFI plugin. Podspec does not support relative
  # paths, so Classes contains a forwarder C file that relatively imports
  # `../src/*` so that the C sources can be shared among all target platforms.
  s.source           = { :path => '.' }
  s.source_files = 'Classes/**/*'
  s.public_header_files = 'Classes**/*.h'
  s.dependency 'Flutter'
  s.platform = :ios, '11.0'
  s.static_framework = true
  s.vendored_libraries =  "**/*.a"

  # Flutter.framework does not contain a i386 slice.
  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES', 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => ['i386','arm64'], "OTHER_LDFLAGS" => "-force_load $(PODS_TARGET_SRCROOT)/libTAK.a -lstdc++" }
  s.swift_version = '5.0'
end
