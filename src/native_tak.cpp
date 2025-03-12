#include "tak.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined TARGET_ANDROID
    #include "environmentProvider.h"
#endif

extern "C" {

#include "native_tak.h"

  // Public methods
  __attribute__((visibility("default"))) __attribute__((used)) 
  int32_t native_initialize(char* path, char* license) {
    JNIEnv *jniEnvironment = NULL;
    jobject context = NULL;
  #if defined TARGET_ANDROID
    getEnvironment((void **)&jniEnvironment);
    getContext(&context);
  #endif

    return TakLib_initialize(path, license, jniEnvironment, context);
  }

  __attribute__((visibility("default"))) __attribute__((used)) 
  void native_release() {
    TakLib_release();
    // TODO: Decide what to do with this
    // #if defined TARGET_ANDROID
    //     releaseEnvironment();
    // #endif
  }

  __attribute__((visibility("default"))) __attribute__((used))
  void native_reset() {
    TakLib_reset();
  }

  __attribute__((visibility("default"))) __attribute__((used)) 
  int32_t native_register(char* user) {
    return TakLib_register(user);
  }

  __attribute__((visibility("default"))) __attribute__((used)) 
  int32_t native_checkIntegrity() {
    return TakLib_checkIntegrity(NULL);
  }

  __attribute__((visibility("default"))) __attribute__((used)) 
  char* native_getTakVersion() {
    char* version;
    TakLib_getTAKVersion(&version);
    return version;
  }

  __attribute__((visibility("default"))) __attribute__((used))
  char* native_getBuildVersion() {
    char* version;
    TakLib_getBuildVersion(&version);
    return version;
  }

  __attribute__((visibility("default"))) __attribute__((used))
  IsRegisteredResponse native_isRegistered() {
    IsRegisteredResponse isRegisteredResponse;
    isRegisteredResponse.returnCode = NULL;
    isRegisteredResponse.isRegistered = TakLib_isRegistered(&(isRegisteredResponse.returnCode));
    
    return isRegisteredResponse;
  }

  __attribute__((visibility("default"))) __attribute__((used))
  bool native_isInitialized() {
    return TakLib_isInitialized();
  }

  __attribute__((visibility("default"))) __attribute__((used))
  TakIdResponse native_getTakId() {
    TakIdResponse takIdResponse;
    takIdResponse.takId = NULL;
    takIdResponse.returnCode = TakLib_getTakIdentifier(&(takIdResponse.takId));

    return takIdResponse;
  }

  __attribute__((visibility("default"))) __attribute__((used)) 
  bool native_getRootStatus() {
    return TakLib_getRootStatus();
  }

  __attribute__((visibility("default"))) __attribute__((used)) 
  TAK_ROOT_STATUS native_getAdvancedRootStatus() {
    return TakLib_getAdvancedRootStatus();
  }

  __attribute__((visibility("default"))) __attribute__((used)) 
  int native_createRuntimeCheckThread(int timeInterval) {
    return TakLib_createRuntimeCheckThread(timeInterval);
  }

  __attribute__((visibility("default"))) __attribute__((used)) 
  int native_stopRuntimeThread() {
    return TakLib_stopRuntimeThread();
  }

  __attribute__((visibility("default"))) __attribute__((used)) 
  bool native_isRuntimeThreadActive(bool relaunch) {
    return TakLib_isRuntimeThreadActive(relaunch);
  }

  __attribute__((visibility("default"))) __attribute__((used))
  TakByteBufferResponse native_fileProtectorDecryptFromFile(char* fileName, char* extension) {
    TakByteBufferResponse response;
    response.returnCode = TAK_GENERAL_ERROR;
    response.buffer.data = NULL;
    response.buffer.length = 0;

    TAK_byte_buffer readValue;
    response.returnCode = TakLib_fileProtectorDecryptFromFile(fileName, extension, &readValue); 
    if (response.returnCode == TAK_SUCCESS) {
        response.buffer.data = (unsigned char*) malloc(readValue.length * sizeof(unsigned char));
        memcpy(response.buffer.data, readValue.data, readValue.length);
        response.buffer.length = readValue.length;
    }
  
    return response;
  } 

  __attribute__((visibility("default"))) __attribute__((used))
  TakByteBufferResponse native_fileProtectorEncrypt(TAK_byte_buffer input) {
    TakByteBufferResponse response;
    response.returnCode = TAK_GENERAL_ERROR;
    response.buffer.data = NULL;
    response.buffer.length = 0;

    TAK_byte_buffer outputValue;
    response.returnCode = TakLib_fileProtectorEncrypt(input, &outputValue);
    if (response.returnCode == TAK_SUCCESS) {
        response.buffer.data = (unsigned char*)malloc(outputValue.length * sizeof(unsigned char));
        if (response.buffer.data != NULL) {
            memcpy(response.buffer.data, outputValue.data, outputValue.length);
            response.buffer.length = outputValue.length;
        }
    }

    return response;
  }

  __attribute__((visibility("default"))) __attribute__((used))
  TakByteBufferResponse native_fileProtectorDecrypt(TAK_byte_buffer input) {
    TakByteBufferResponse response;
    response.returnCode = TAK_GENERAL_ERROR;
    response.buffer.data = NULL;
    response.buffer.length = 0;

    TAK_byte_buffer outputValue;
    response.returnCode = TakLib_fileProtectorDecrypt(input, &outputValue);
    if (response.returnCode == TAK_SUCCESS) {
        response.buffer.data = (unsigned char*)malloc(outputValue.length * sizeof(unsigned char));
        if (response.buffer.data != NULL) {
            memcpy(response.buffer.data, outputValue.data, outputValue.length);
            response.buffer.length = outputValue.length;
        } 
    }

    return response;
  }

  __attribute__((visibility("default"))) __attribute__((used))
  int32_t native_storageCreate(char* storageName) {  
    return TakLib_storageCreate(storageName);
  }

  __attribute__((visibility("default"))) __attribute__((used))
  int32_t native_storageDelete(char* storageName) {
    return TakLib_storageDelete(storageName);
  }

  __attribute__((visibility("default"))) __attribute__((used))
  int32_t native_storageWrite(char* storageName, char* key, unsigned char* value, int valueLength) { 
    TAK_byte_buffer valueToStore;
    valueToStore.length = valueLength;
    valueToStore.data = value;
    return TakLib_storageWrite(storageName, key, valueToStore);
  }

  __attribute__((visibility("default"))) __attribute__((used))
  TakByteBufferResponse native_storageRead(char* storageName, char* key) {      
    TakByteBufferResponse response;
    response.returnCode = TAK_GENERAL_ERROR;
    response.buffer.data = NULL;
    response.buffer.length = 0;
    
    TAK_byte_buffer readValue;
    response.returnCode = TakLib_storageRead(storageName, key, &readValue);
    if (response.returnCode == TAK_SUCCESS) {
      response.buffer.data = (unsigned char*)malloc(readValue.length * sizeof(unsigned char));
      if (response.buffer.data != NULL) {
        memcpy(response.buffer.data, readValue.data, readValue.length);
        response.buffer.length = readValue.length;
      } 
    }
    
    return response;
  }
   
  __attribute__((visibility("default"))) __attribute__((used))
  int32_t native_storageDeleteEntry(char* storageName, char* key) {
    return TakLib_storageDeleteEntry(storageName, key);
  }

}
