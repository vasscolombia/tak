#ifndef NATIVE_TAK_HEADER
#define NATIVE_TAK_HEADER

#include "tak.h"
#include <stdint.h>

typedef struct {
    char* takId;
    int returnCode;
} TakIdResponse;

typedef struct {
    bool isRegistered;
    int returnCode;
} IsRegisteredResponse;

typedef struct {
    int socketDescriptor;
    char* peerCertificate;
    int returnCode;
} TlsConnectionResponse;

typedef struct {
    int32_t returnCode;
    TAK_byte_buffer buffer;
} TakByteBufferResponse;

// Native methods
int32_t native_initialize(char *path, char *license);
void native_release();
void native_reset();
int32_t native_register(char *user);
IsRegisteredResponse native_isRegistered();
char* native_getBuildVersion();
char* native_getTakVersion();
int32_t native_checkIntegrity();
TakIdResponse native_getTakId();
bool native_getRootStatus();
TAK_ROOT_STATUS native_getAdvancedRootStatus();
int native_stopRuntimeThread();
int native_createRuntimeCheckThread(int timeInterval);
bool native_isRuntimeThreadActive(bool relaunch);
TakByteBufferResponse native_fileProtectorDecryptFromFile(char* fileName,char* extension);
TakByteBufferResponse native_fileProtectorEncrypt(TAK_byte_buffer input);
TakByteBufferResponse native_fileProtectorDecrypt(TAK_byte_buffer input);
int32_t native_storageCreate(char* storageName);
int32_t native_storageDelete(char* storageName);
int32_t native_storageWrite(char* storageName, char* key, unsigned char* value, int valueLength);
TakByteBufferResponse native_storageRead(char* storageName, char* key);
TlsConnectionResponse native_tlsConnectSecurePinning(const char *fqdn, const char *port, unsigned int timeout);
int native_tlsClose(int socketDescriptor);
TakByteBufferResponse native_tlsReadAll(int socketDescriptor);
TakByteBufferResponse native_tlsRead(int socketDescriptor, int length);
int32_t native_tlsWrite(int socketDescriptor,unsigned char* bufferData);
bool native_tlsIsClosed(int socketDescriptor);
int32_t native_tlsClose(int socketDescriptor);

// VASS
TakByteBufferResponse native_getPinnedCertificate(const char* hostName);
int native_updatePinnedCertificates();

#endif // #ifdef NATIVE_TAK_HEADER
