package com.build38.tak.flutter

import android.app.Activity
import android.content.Context
import android.content.ContextWrapper

import androidx.annotation.NonNull

import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel
import io.flutter.plugin.common.MethodChannel.MethodCallHandler
import io.flutter.plugin.common.MethodChannel.Result

/**
 * Class for setting up Flutter Method Channel.
 *
 */
class TakPlugin: FlutterPlugin, MethodCallHandler {

  private lateinit var channel : MethodChannel

  private lateinit var context: Context

    companion object {
        init {
            System.loadLibrary("tak_flutter_wrapper")
        }
    }

    /**
     * Sets up MethodChannel, and gets the application context.
     */
    override fun onAttachedToEngine(flutterPluginBinding: FlutterPlugin.FlutterPluginBinding) {
        channel = MethodChannel(flutterPluginBinding.binaryMessenger, "tak")
        channel.setMethodCallHandler(this)
        context = flutterPluginBinding.applicationContext
    }

    /**
     * Loads TAK environment and returns the working directory.
     */
    override fun onMethodCall(call: MethodCall, result: Result) {
        if (call.method == "loadEnvironment") {
            loadEnvironment(ContextWrapper(context))
            val workingPath = context.filesDir.absolutePath
            result.success(workingPath)
        } 
        else {
            result.notImplemented()
        }
    }

    /**
     * Detaches Method Channel.
     */
    override fun onDetachedFromEngine(binding: FlutterPlugin.FlutterPluginBinding) {
        channel.setMethodCallHandler(null)
    }

    private external fun loadEnvironment(context: ContextWrapper): Int
}