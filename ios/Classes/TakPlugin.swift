import Flutter
import UIKit

public class TakPlugin: NSObject, FlutterPlugin {
  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "tak", binaryMessenger: registrar.messenger())
    let instance = TakPlugin()
    registrar.addMethodCallDelegate(instance, channel: channel)
  }

  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
    switch call.method {
    case "loadEnvironment":
      let libraryDir = NSSearchPathForDirectoriesInDomains(.libraryDirectory, .userDomainMask, true)[0]
      result(libraryDir)
    default:
      result(FlutterMethodNotImplemented)
    }
  }
}
