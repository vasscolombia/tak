/*****************************************************************************/
/**
@file
@brief Build38 GmbH - Trusted Application Kit - API

This software is provided to you pursuant to your Software
License Agreement (SLA) with Build38 GmbH.
This software may be used only in accordance with the
terms of this agreement.

T.A.K-Client uses third-party software according to their licences.
The licences can be found in the T.A.K delivery.

Copyright (c) 2021, Build38 GmbH. All rights reserved.

*/

#ifndef _TAK_H_
#define _TAK_H_

//** **************************************************************************
//** ***************************   INCLUDES   *********************************
//** **************************************************************************
#include <stddef.h>
#include <stdbool.h>

#ifdef TARGET_ANDROID
#include <jni.h>
#else
typedef void JNIEnv;    /**< \internal */
typedef void* jobject;  /**< \internal */
#endif

//** **************************************************************************
//** ***************************   DEFINES   *********************************
//** **************************************************************************


//! Marker for a cryptographic key type.
/*!
 */
typedef enum {
    WRAPPED_KEY,            /*!< The key object is a 'wrapped key' - a key encrypted with the key encryption key */
    WBC_EXPORTED_OBJECT,    /*!< The key object is an 'WBC exported key' - a special key format of G+D WBC */
    WBC_COMMON_OBJECT,      /*!< The key object is a 'WBC key' - a special key format of G+D WBC */
    HARDWARE_BACKED_REF,    /*!< The key object is a reference to an 'Android KeyStore Hardware-Backed key' - used to reference the key in the secure environment */
    STRONGBOX_REF,          /*!< The key object is a reference to an 'Android StrongBox key' - used to reference the key in the secure environment */
    SECURE_ENCLAVE_REF      /*!< The key object is a reference to an 'iOS Secure Enclave key' - used to reference the key in the secure environment */
} TAK_KEY_TYPE;

//! Enum defining the available key algorithms.
/*!
 */
typedef enum {
    KEY_ALGO_AES_256,
    KEY_ALGO_RSA_2048, /*\deprecated Since version 2.8.0. Consider to migrate to EC license instead. */
    KEY_ALGO_RSA_1024, /*\deprecated Not to be used. Not supported since version 2.17.0, kept the enum for compatibility. */
    KEY_ALGO_EC_SECP256R1,
    KEY_ALGO_AES_128 /* Use only if KEY_ALGO_AES_256 is not supported in your use case. */
} TAK_KEY_ALGORITHM;

//! Enum defining the supported signature algorithms.
/*!
 */
typedef enum {
    SIGN_ALGO_RSA_PKCS_1_V15, /*\deprecated Since version 2.8.0. Use #TakLib_hashAndSign instead. */
    SIGN_ALGO_ECDSA
} TAK_SIGNATURE_ALGORITHM;

//! Enum defining the supported hash types.
/*!
 */
typedef enum {
    HASH_ALGO_NONE,
    HASH_ALGO_SHA224,
    HASH_ALGO_SHA256,
    HASH_ALGO_SHA384,
    HASH_ALGO_SHA512
} TAK_HASH_ALGORITHM;

//! Structure defining a byte buffer.
/*!
 */
typedef struct {
    unsigned char* data;            /*!< Pointer to the byte array. After the usage of an instance of TAK_byte_buffer, .data bust be freed using the function free(). If not, it will cause a memory leak. */
    unsigned int   length;          /*!< Length of the byte array */
} TAK_byte_buffer;

//! Structure defining the key information used by the T.A.K-Client library.
/*!
 */
typedef struct {
    const char* alias;              /*!<  String alias. */
    TAK_KEY_ALGORITHM algorithm;    /*!< Algorithm type */
    TAK_KEY_TYPE protectionLevel;   /*!< Cryptographic key type. */
} TAK_KEY_INFO;

//! Structure defining the supported encryption algorithms used by the T.A.K-Client library.
/*!
 */
typedef enum {
    ENCRYPT_ALGO_AES_GCM,
    ENCRYPT_ALGO_AES_ECB, /*!< ECB block mode is not supported by HwBacked cryptography. */
    ENCRYPT_ALGO_AES_CBC, /*!< CBC block mode is not supported by HwBacked cryptography. */
    ENCRYPT_ALGO_ECIES
} TAK_ENCRYPTION_ALGORITHM;

typedef enum {
    TAK_PADDING_NONE,                                                                      /**< Cipher object without padding */
    TAK_PADDING_PKCS_5 __attribute__((deprecated("Please refer to the Developer Documentation."))),    /**< Cipher object with PKCS#5 padding */
} TAK_PADDING_TYPE;

//! Represents the output of an encryption operation, regardless of the algorithm.
/*! Presence of some of the fields is optional and is determined by the encryption algorithm used.
 */
typedef struct {
    TAK_byte_buffer iv; /*!<  Initialization Vector. Only if applicable, e.g. on AES-GCM encryption schemes. */
    TAK_byte_buffer aad; /*!<  Additional Authentication Data. Only if applicable, e.g. on AES-GCM encryption schemes. */
    TAK_byte_buffer tag; /*!<  Authentication tag. Only if applicable, e.g. on AES-GCM encryption schemes. */
    TAK_byte_buffer ephemeralKey; /*!<  Ephemeral public key. Only if applicable, e.g. on ECIES encryption schemes. */
    TAK_byte_buffer ciphertext; /*!<  The encrypted data. */
} TAK_ENCRYPTION_OUTPUT;

//! Structure defining the different types of detection root status used by the T.A.K-Client library.
/*!
 */
typedef enum {
    NOT_ROOTED,
    MAYBE_ROOTED,
    LIKELY_ROOTED,
    ROOTED
} TAK_ROOT_STATUS;


//! Structure defining the supported wrapping algorithms used by the T.A.K-Client library.
/*!
 */
typedef enum {
    WRAPPING_ALGO_AES_CBC 
} TAK_WRAPPING_ALGORITHM;

 
//! Structure that represents the key in wrapped format.
typedef struct {
    TAK_ENCRYPTION_OUTPUT wrappedKeyData;  /*!<  The wrapped key data. */
    TAK_KEY_ALGORITHM keyAlgorithm; /*!<  The key algorithm. */
    TAK_WRAPPING_ALGORITHM wrappingAlgorithm; /*!<  The algorithm used for wrapping. */
    char* wrappingKeyAlias;  /*!<  The alias of the key used for wrapping. */
} TAK_WRAPPED_KEY;

//! Structure that represents the runtime crash reasons.
/*!
 */
typedef enum {
    RUNTIME_DEBUGGER_DETECTED,
    RUNTIME_ROOT_DETECTED,
    RUNTIME_HOOKING_DETECTED,
    RUNTIME_EMULATOR_DETECTED,
    RUNTIME_APP_REPACKAGING_DETECTED,
    RUNTIME_TAMPERING_DETECTED
} TAK_RUNTIME_CRASH_REASON;

/** All T.A.K LIB functions return a value of type #TAK_RETURN  */
/** \defgroup TAK_RETURN_VALUES T.A.K Return Values
 * \{
 */

//! Data type for return values of all public functions within the T.A.K API
typedef int TAK_RETURN;

///@name All possible values of TAK_RETURN
///@{

/**
 * @brief No error was encountered.
 */
#define TAK_SUCCESS                     ((int)0x00000000)

/**
 * @brief [DEPRECATED] No error was encountered, but the validation process had to be repeated to update device information in the T.A.K Cloud.
 *
 * \deprecated Since version 2.2.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_UPDATE_SUCCESS              ((int)0x00000001)

/**
 * @brief Re-registration finished successfully. T.A.K ID and key material have been updated.
 */
#define TAK_RE_REGISTER_SUCCESS         ((int)0x00000002)

/**
 * @brief A general error occurred, no further diagnosis.
 */
#define TAK_GENERAL_ERROR               ((int)0x000F0001)

/**
 * @brief [DEPRECATED] No memory available to finish the operation.
 *
 * \deprecated Since version 2.2.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_OUT_OF_MEMORY               ((int)0x000F0002)

/**
 * @brief A given parameter is null or invalid.
 */
#define TAK_INVALID_PARAMETER           ((int)0x000F0003)

/**
 * @brief The calling thread is not the UI Thread or the one with the ownership of the process.
 */
#define TAK_INVALID_EXECUTION_THREAD    ((int)0x000F0004)

/**
 * @brief The server response was not as expected.
 */
#define TAK_INVALID_SERVER_RESPONSE     ((int)0x000F0005)

/**
 * @brief [DEPRECATED] Multithreading error.
 *
 * \deprecated Since version 2.2.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_MULTI_THREAD_ERROR          ((int)0x000F0006)

/**
 * @brief The module is not available in this T.A.K configuration.
 */
#define TAK_NOT_AVAILABLE               ((int)0x000FFFFF)

/**
 * @brief Library was already initialized.
 */
#define TAK_API_ALREADY_INITIALIZED     ((int)0x00010001)

/**
 * @brief Library not initialized.
 */
#define TAK_API_NOT_INITIALIZED         ((int)0x00010002)

/**
 * @brief Device validation process failed.
 */
#define TAK_DEVICE_VALIDATION_FAILED    ((int)0x00020002)

/**
 * @brief Device update process failed.
 */
#define TAK_DEVICE_UPDATE_FAILED        ((int)0x00020003)

/**
 * @brief [DEPRECATED] Device update is necessary (send missing identifiers to the T.A.K server).
 *
 * \deprecated Since version 2.2.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_DEVICE_UPDATE_NEEDED        ((int)0x00020004)

/**
 * @brief Device fingerprint could not be retrieved.
 */
#define TAK_DEVICE_NO_FINGERPRINT       ((int)0x00020005)

/**
 * @brief The license has expired. Library cannot be used anymore.
 */
#define TAK_LICENSE_EXPIRED             ((int)0x00020006)

/**
 * @brief The license is about to expire.
 * Renew the license file or it will stop working.
 * Normally, this means that the user needs to update the app.
 */
#define TAK_LICENSE_ABOUT_TO_EXPIRE     ((int)0x00020007)

/**
 * @brief The client certificate has expired, so re-registration is not possible anymore.
 * A new registration needs to be carried out.
 * Use TakLib_reset() to clear user data and use TakLib_register(...) to set up the library again.
 */
#define TAK_CLIENT_CERTIFICATE_EXPIRED  ((int)0x00020008)

/**
 * @brief Library was already registered.
 */
#define TAK_ALREADY_REGISTERED          ((int)0x00020009)

/**
 * @brief Library has not yet been registered, which is required to finish the requested operation.
 */
#define TAK_NOT_REGISTERED              ((int)0x0002000A)

/**
 * @brief Application has been remotely wiped.
 * This action has been triggered from the backend side.
 * After an app instance is wiped, all information stored in it will be permanently delete.
 * This includes instance information (such as T.A.K ID,keys and certificates) as well as any information in the Secure Storage.
 * Notice that, after a remote wipe, TakLib_isRegistered() will return "false".
 */
#define TAK_INSTANCE_WIPED              ((int)0x0002000B)

/**
 * @brief Application has been remotely locked.
 * This action has been triggered from the backend side.
 * After an app instance is locked some T.A.K methods will refuse to execute.
 * The state is reversible with a remotely unlocked request.
 */
#define TAK_INSTANCE_LOCKED              ((int)0x0002000C)

/**
 * @brief [DEPRECATED] General error in security module.
 *
 * \deprecated Since version 2.4.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_SECURITY_GENERAL_ERROR      ((int)0x00030001)

/**
 * @brief [DEPRECATED] Error in socket operations.
 *
 * \deprecated Since version 2.2.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_SECURITY_SOCKET_ERROR       ((int)0x00030002)

/**
 * @brief [DEPRECATED] Error in TLS setup.
 *
 * \deprecated Since version 2.2.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_SECURITY_SSL_ERROR          ((int)0x00030003)

/**
 * @brief [DEPRECATED] Error when reading from TLS stream.
 *
 * \deprecated Since version 2.2.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_SECURITY_SSL_READ_ERROR     ((int)0x00030004)

/**
 * @brief [DEPRECATED] Error when writing to TLS stream.
 *
 * \deprecated Since version 2.2.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_SECURITY_SSL_WRITE_ERROR    ((int)0x00030005)

/**
 * @brief Invalid certificate or unable to use certificate.
 */
#define TAK_SECURITY_CERTIFICATE_ERROR  ((int)0x00030006)

/**
 * @brief [DEPRECATED] Invalid key or unable to use key.
 *
 * \deprecated Since version 2.2.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_SECURITY_KEY_ERROR          ((int)0x00030007)

/**
 * @brief Timeout during TLS read or write operation.
 */
#define TAK_NETWORK_TIMEOUT             ((int)0x00030008)

/**
 * @brief Error during network communication.
 */
#define TAK_NETWORK_ERROR               ((int)0x00030009)

/**
 * @brief [DEPRECATED] Error initializing whitebox system.
 *
 * \deprecated Since version 2.3.0. This code is never returned by the T.A.K client SDK and should not be used. Use TAK_GENERAL_ERROR instead.
 */
#define TAK_WBC_ERROR                   ((int)0x00040001)

/**
 * @brief [DEPRECATED] Error in crypto operation.
 *
 * \deprecated Since version 2.4.0. This code is never returned by the T.A.K client SDK and should not be used. Use TAK_GENERAL_ERROR instead.
 */
#define TAK_CRYPTO_ERROR                ((int)0x00040001)

/**
 * @brief [DEPRECATED] General error with the secure storage.
 *
 * \deprecated Since version 2.2.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_STORAGE_ERROR               ((int)0x00050001)

/**
 * @brief A storage object by this name does not exist.
 */
#define TAK_STORAGE_NOT_FOUND           ((int)0x00050002)

/**
 * @brief Key identifier not found inside the storage object.
 */
#define TAK_STORAGE_KEY_NOT_FOUND       ((int)0x00050003)

/**
 * @brief Storage object by this name already exists.
 */
#define TAK_STORAGE_ALREADY_EXISTS      ((int)0x00050004)

/**
 * @brief App appears to be running on a different device.
 */
#define TAK_STORAGE_DEVICE_MISMATCH     ((int)0x00050005)

/**
 * @brief [DEPRECATED] General error in the keychain storage helper (Only for iOS).
 *
 * \deprecated Since version 2.2.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_KEYCHAIN_ERROR              ((int)0x00060001)

/**
 * @brief [DEPRECATED] Error when storing values in the keychain (Only for iOS).
 *
 * \deprecated Since version 2.2.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_KEYCHAIN_SET_ERROR          ((int)0x00060002)

/**
 * @brief [DEPRECATED] Error when obtaining values in the keychain (Only for iOS).
 *
 * \deprecated Since version 2.2.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_KEYCHAIN_GET_ERROR          ((int)0x00060003)

/**
 * @brief [DEPRECATED] Error when removing values in the keychain (Only for iOS).
 *
 * \deprecated Since version 2.2.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_KEYCHAIN_REMOVE_ERROR       ((int)0x00060004)

/**
 * @brief [DEPRECATED] Error when generating signature (Only for iOS).
 *
 * \deprecated Since version 2.2.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_SECURITY_GEN_SIG_ERROR      ((int)0x00070001)

/**
 * @brief [DEPRECATED] Error when generating keypairs (Only for iOS).
 *
 * \deprecated Since version 2.2.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_KEYPAIR_GEN_ERROR           ((int)0x00070002)

/**
 * @brief [DEPRECATED] Error when storing key (Only for iOS).
 *
 * \deprecated Since version 2.2.0. This code is never returned by the T.A.K client SDK and should not be used.
 */
#define TAK_KEY_STORE_ERROR             ((int)0x00070003)

///@}

/** \} */

/** Pre-defined aliases for referring to T.A.K-internal cryptographic keys. */
/** \defgroup TAK_KEY_ALIASES T.A.K Keys Alias
 * \{
 */

/**
 * @brief Alias for the Default Client Private Key.
 */
#define KEY_ALIAS_DEFAULT_CLIENT_PRIVATE_KEY "TNSV9ZjNj18TSqAdjEYX8YVFiiIDbYuNuOpvpcb7X5M"

/**
 * @brief Alias for the Default Client Encryption Key.
 */
#define KEY_ALIAS_DEFAULT_CLIENT_ENCRYPTION_KEY "aWNYy53UYmq6p06SyMP7BEksokZbZYNIBXkY17sH_6s"

/**
 * @brief Alias for the Individual Client Private Key.
 */
#define KEY_ALIAS_INDIVIDUAL_CLIENT_PRIVATE_KEY "XyULLpIxdfSx_NLSRgxYUNXG2os0iRSfHcAxwy_tss8"

/**
 * @brief Alias for the Individual Client Encryption Key.
 */
#define KEY_ALIAS_INDIVIDUAL_CLIENT_ENCRYPTION_KEY "fc5LRoR8IOSKGUYa5K9rvYMiuUPtakcMlgpidiXywlY"

/** \} */

#ifdef __cplusplus
extern "C" {
#endif

//** **************************************************************************
//** *****************************   API   ************************************
//** **************************************************************************

/**
 * \defgroup TAKBUILD T.A.K Build
 *
 * This functions provide information about the T.A.K build being used.
 */

/**
 * \defgroup TAKSESSION T.A.K Session
 *
 * A T.A.K session needs to be created before using most of the functions of T.A.K.
 *
 * A session is created by calling #TakLib_initialize, and must be destroyed using #TakLib_release.
 */


/******************************************************************************
*/ /**
@brief Initializes the T.A.K library

This function has to be called first before any other operation is possible.\n
The workingPath has to point to a directory in the filesystem where the T.A.K library has permission to read and write.

@ingroup TAKSESSION

@param[in] workingPath          String to the directory that is writable for the application to store persistent data.
                            Mandatory.
@param[in] licenseFilePath         Path (either relative or absolute) to the location of the licenseKey.
                                For Android, it can be either:
                                - The absolute path to the license file, or
                                - The name of the license file, which has been bundled in the "assets" dir of an APK
                                Mandatory.
@param[in] env         A reference to the JNI environment. Mandatory for Android since version 2.3.0.
@param[in] contextWrapper      \parblock
                            An instance of the class android.content.ContextWrapper (or a subclass thereof, e.g.
                            android.app.Application).

                            It is not recommended to provide an Activity-bound context and instead use the application-wide
                            context. If you choose to provide an Activity context, make sure to release T.A.K before
                            using it in a different Activity.

                            Mandatory for Android since version 2.3.0.
                            \endparblock

@retval TAK_SUCCESS                 If the initialization of the API was successful.
@retval TAK_API_ALREADY_INITIALIZED If the API was previously initialized.
@retval TAK_INVALID_PARAMETER       If any input values are invalid.
@retval TAK_LICENSE_ABOUT_TO_EXPIRE If current date is inside grace period.
@retval TAK_LICENSE_EXPIRED         If grace period is over and library needs to be updated.
@retval TAK_GENERAL_ERROR           If an unexpected error has happened.
@retval TAK_INSTANCE_LOCKED         This is a warning, the application has been remotely locked. Some of the functionalities will not be
                                    available until instance is unlocked.

@return The function returns a value of #TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_initialize(const char* workingPath,
                             const char* licenseFilePath,
                             JNIEnv *env,
                             jobject contextWrapper);

/******************************************************************************
*/ /**
@brief Checks whether the T.A.K lib has already been initialized.

@ingroup TAKSESSION

@retval true         T.A.K lib has already been initialized
@retval false         T.A.K lib has not been initialized

@return The function returns true or false.
******************************************************************************/
bool TakLib_isInitialized(void);

/******************************************************************************
*/ /**
@brief Returns the software version of the T.A.K-Client library.

This is the "official" T.A.K lib version, compliant with semantic versioning.\n
This function can be called without previously calling #TakLib_initialize().

@ingroup TAKBUILD

@param[out] pVersion         C-string containing the version string of the
                         T.A.K-Client library (memory must be freed after use)

@retval TAK_SUCCESS         If the T.A.K version was retrieved correctly.
@retval TAK_INVALID_PARAMETER         If input value is invalid.
@retval TAK_GENERAL_ERROR         If an unexpected error has happened.

@return The function returns a value of #TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_getTAKVersion(char** pVersion);


/******************************************************************************
*/ /**
@brief Returns the build version of the T.A.K-Client library.

This is the version number transmitted to the T.A.K server on "register" and "validate" calls. Due to the design of the
T.A.K, this number changes for every T.A.K-Client build, even if there are no code changes.\n
This function can be called without previously calling #TakLib_initialize().

@ingroup TAKBUILD

@param[out] pVersion         C-string containing the build version string of the
                         T.A.K-Client library (memory must be freed after use)

@retval TAK_SUCCESS         If the build version was retrieved correctly.
@retval TAK_INVALID_PARAMETER         If input value is invalid.
@retval TAK_GENERAL_ERROR         If an unexpected error has happened.

@return The function returns a value of #TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_getBuildVersion(char** pVersion);

/******************************************************************************
*/ /**
@brief Releases the claimed resources.

This method has to be called to release all internal references or objects
that might still be open. Any threads still running in the background will
be stopped.

@ingroup TAKSESSION

@retval TAK_SUCCESS         If the release of the API instances was successful
@retval TAK_API_NOT_INITIALIZED         If the API was not previously initialized

@return The function returns a value of #TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_release(void);


//** **************************************************************************
//** **************************************************************************

/**
 * \defgroup TAK_Device T.A.K Instance
 *
 * A T.A.K instance is created during #TakLib_register, when it is assigned a T.A.K ID and a client certificate.
 *
 * Make sure to regularly use #TakLib_checkIntegrity to keep it up-to-date.
 *
 * The instance (and it's associated data) can be deleted by using #TakLib_reset.
 */

/******************************************************************************
*/ /**
@brief Registers the T.A.K lib with the T.A.K server.

@ingroup TAK_Device

@param[in] userHash         Hexadecimal string uniquely identifying the app user, can be NULL

@retval TAK_SUCCESS                     If registration succeeded.
@retval TAK_API_NOT_INITIALIZED         If the API was not previously initialized.
@retval TAK_INVALID_PARAMETER           If input value is invalid.
@retval TAK_ALREADY_REGISTERED          If this method has already been successfully executed in this device.
@retval TAK_LICENSE_ABOUT_TO_EXPIRE     If the license expiry date is close (registration completed suyccessfully).
@retval TAK_LICENSE_EXPIRED             If the license expiry date has been reached (registration could not be performed).
@retval TAK_DEVICE_NO_FINGERPRINT       If device fingerprint could not be retrieved.
@retval TAK_NOT_AVAILABLE               If the server response 404 or cert has expired.
@retval TAK_INVALID_SERVER_RESPONSE     If the server response was not as expected.
@retval TAK_GENERAL_ERROR               If an unexpected error has happened.
@retval TAK_NETWORK_TIMEOUT             If connection to the T.A.K server times out.
@retval TAK_NETWORK_ERROR               If a network error happens during communication.
******************************************************************************/
TAK_RETURN TakLib_register(const char* userHash);

/******************************************************************************
*/ /**
@brief Checks whether the T.A.K lib has already been registered at the T.A.K server.

To be used only together with TakLib_register.

@ingroup TAK_Device

@param[out] error         T.A.K error code

@retval true          T.A.K lib has already been registered
@retval false         T.A.K lib has not been registered

@return The function returns true or false.
******************************************************************************/
bool TakLib_isRegistered(TAK_RETURN *error);


/******************************************************************************
*/ /**
@brief Resets the library state in order to "undo" a registration.
All corresponding data and keys will be deleted!

@ingroup TAK_Device

@retval TAK_SUCCESS                     If the library was reset correctly.
@retval TAK_API_NOT_INITIALIZED         If the API was not previously initialized.
@retval TAK_GENERAL_ERROR               If a general error occurs during the process.
@retval TAK_INSTANCE_LOCKED             If the application has been remotely locked. This method is unavailable until the instance is unlocked.

@return The function returns a value of type TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_reset(void);

/******************************************************************************
*/ /**
@brief Returns the T.A.K client identifier assigned when the T.A.K lib was registered. The T.A.K identifier has the format of a version 4 UUID.

To be used only together with TakLib_register.
    
After TakLib_getTakIdentifier returns, the used variable sent as takId must be freed using the dynamic memory function free(). If no free is called,
a memory leak will be generated.

@ingroup TAK_Device

@param[out] takId          T.A.K identifier

@retval TAK_SUCCESS               If the identifier was retrieved correctly.
@retval TAK_GENERAL_ERROR         If general error during process.
@retval TAK_INVALID_PARAMETER     If input value is invalid.
@retval TAK_NOT_REGISTERED        If key is not available because register step is missing.
@retval TAK_API_NOT_INITIALIZED   If the API was not previously initialized.
@retval TAK_INSTANCE_LOCKED       If the application has been remotely locked. This method is unavailable until the instance is unlocked.

@return The function returns a value of type TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_getTakIdentifier(char** takId);

/******************************************************************************
*/ /**
@brief [DEPRECATED] Returns the client certificate assigned when the T.A.K lib was (re-)registered.

To be used only together with TakLib_register. Memory must be freed after use.

\deprecated Since version 2.7.0. Use #TakLib_getCertificatePemFormatByAlias instead.

@ingroup TAK_Device

@param[out] cert        T.A.K client certificate

@retval TAK_SUCCESS                    If the certificate was retrieved correctly.
@retval TAK_INVALID_PARAMETER          If input value is invalid.
@retval TAK_NOT_REGISTERED             If key is not available because register step is missing.
@retval TAK_API_NOT_INITIALIZED        If the API was not previously initialized.
@retval TAK_INSTANCE_LOCKED            If the application has been remotely locked. This method is unavailable until the instance is unlocked.

@return The function returns a value of type TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_getClientCertificate(char **cert) __attribute((deprecated("Use TakLib_getCertificatePemFormatByAlias instead")));

/******************************************************************************
*/ /**
@brief Returns the client certificate for a given key alias.

In case the alias is the individual key "KEY_ALIAS_INDIVIDUAL_CLIENT_PRIVATE_KEY" it must be used only together with TakLib_register.
Memory must be freed after use.

@ingroup TAK_Device

@param[in] keyAlias         Alias of the key that we will retrieve the certificate from.
@param[out] certificate     Certificate of a given key alias.

@retval TAK_SUCCESS                    If the certificate was retrieved correctly.
@retval TAK_INVALID_PARAMETER          If input value is invalid.
@retval TAK_NOT_REGISTERED             If alias is the individual key, the certificate is not available because the register step is missing.
@retval TAK_API_NOT_INITIALIZED        If the API was not previously initialized.
@retval TAK_INSTANCE_LOCKED            If the application has been remotely locked. This method is unavailable until the instance is unlocked.

@return The function returns a value of type TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_getCertificatePemFormatByAlias(const char *keyAlias, char **certificate);

/**
 * \defgroup TAK_CRYPTO Crypto Utils
 *
 * Set of functions that provide cryptographic utilities.
 */

/******************************************************************************
*/ /**
@brief [DEPRECATED] Generates an RSA signature with the individual client private key received during registration.

This method calculates a SHA256 hash of the given data and signs the hash value
as standardized in version 1.5 of PKCS #1.

To be used only together with TakLib_register.
    
Please, remember that if this method is executed in a device that is using HW-Backed keys it will not be available,
because HW-Backed keys use EC crypto. Use #TakLib_hashAndSign instead.

\deprecated Since version 2.4.0. Use #TakLib_hashAndSign instead.

@ingroup TAK_CRYPTO

@param[in] data        Buffer containing the data to hash and then sign
@param[in] expectedSignatureLength        Expected output length of the signature (must be 256 for RSA 2048 signatures)
@param[out] signature        Output buffer to hold the generated signature and size of signature buffer


@retval TAK_SUCCESS                    Signature generation was successful
@retval TAK_API_NOT_INITIALIZED        If the API was not previously initialized.
@retval TAK_NOT_REGISTERED             If key is not available because register step is missing.
@retval TAK_INVALID_PARAMETER          If input value is invalid.
@retval TAK_NOT_AVAILABLE              If the device supports HW-backed cryptography this method will return this error code, use #TakLib_hashAndSign instead.
@retval TAK_GENERAL_ERROR              If general error during process.
@retval TAK_INSTANCE_LOCKED            If the application has been remotely locked. This method is unavailable until the instance is unlocked.

@return The function returns a value of #TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_generateSignature(TAK_byte_buffer data,
                                    unsigned int expectedSignatureLength,
                                    TAK_byte_buffer* signature)
__attribute((deprecated("Use TakLib_hashAndSign instead")));

/******************************************************************************
*/ /**
@brief [DEPRECATED] Generates an RSA signature with the default client private key. No previous registration is required nor internet
       access.

This method calculates a SHA256 hash of the given data and signs the hash value
as standardized in version 1.5 of PKCS #1.

\deprecated Since version 2.4.0. Use #TakLib_hashAndSign instead.

@ingroup TAK_CRYPTO

@param[in] data        Buffer containing the data to hash and then sign.
@param[in] signatureLength        Expected output length of the signature (must be 256 for RSA 2048 signatures).
@param[out] signature        Output buffer to hold the generated signature and size of signature buffer.

@retval TAK_SUCCESS                 Signature generation was successful.
@retval TAK_API_NOT_INITIALIZED     If the API was not previously initialized.
@retval TAK_INVALID_PARAMETER       If input value is invalid.
@retval TAK_NOT_AVAILABLE           If key algorithm not supported.
@retval TAK_GENERAL_ERROR           If general error during process.
@retval TAK_INSTANCE_LOCKED         If the application has been remotely locked. This method is unavailable until the instance is unlocked.

@return The function returns a value of #TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_generateSignatureWithDefaultKey(TAK_byte_buffer data,
                                                  unsigned int signatureLength,
                                                  TAK_byte_buffer* signature)

__attribute((deprecated("Use TakLib_hashAndSign instead")));

/******************************************************************************
*/ /**
@brief Syncs the device state with the T.A.K server. If the T.A.K client certificate is about to expire, a re-registration
Sis performed.
Note: A re-registration will change the T.A.K identifier!

To be used only together with TakLib_register.

@ingroup TAK_Device

@param[in] safetynetAttestation        C-string containing a SafetyNet attestation result if available or NULL.

@retval TAK_SUCCESS                    CheckIntegrity was successful.
@retval TAK_API_NOT_INITIALIZED        If the API was not previously initialized.
@retval TAK_INVALID_PARAMETER          If input parameter is invalid.
@retval TAK_GENERAL_ERROR              If general error during process.

@retval TAK_RE_REGISTER_SUCCESS        Re-registration against T.A.K server finished successfully. T.A.K ID and key material have been updated.

@retval TAK_NOT_REGISTERED             App first needs to register the T.A.K lib before calling TakLib_checkIntegrity

@retval TAK_CLIENT_CERTIFICATE_EXPIRED App needs to register again
@retval TAK_LICENSE_ABOUT_TO_EXPIRE    App should warn the user that the app should be updated in the next days
@retval TAK_LICENSE_EXPIRED            App should warn the user that the app must be updated now in order to be usable

@retval TAK_STORAGE_DEVICE_MISMATCH    App appears to be running on a different device, user data has been deleted
                                       (meaning that devices is not registered anymore).
@retval TAK_INSTANCE_WIPED             Application has been remotely wiped. This action has been triggered from the backend side.
                                       All the app instance is wiped, all information stored in has been permanently deleted.
                                       This includes instance information (such as T.A.K ID,keys and certificates) as well as any information in the Secure Storage.
                                       Notice that, after a remote wipe, TakLib_isRegistered() will return "false".
@retval TAK_INSTANCE_LOCKED            This is a warning, the application has been remotely locked. Some of the functionalities will not be available until instance is unlocked.
@retval TAK_DEVICE_NO_FINGERPRINT      If device fingerprint could not be retrieved.
@retval TAK_NETWORK_TIMEOUT            App should try again later
@retval TAK_NETWORK_ERROR              App should try again later

@return The function returns the result of the transaction.
******************************************************************************/
TAK_RETURN TakLib_checkIntegrity(const char* safetynetAttestation);


/******************************************************************************
*/ /**
@brief Generates a specific length nonce (number used once)

Generates a nonce to be used by the app with the next SafetyNet attestation API call. The nonce will be stored
internally and compared to the nonce contained in the attestation result parameter of the next call to
TakLib_checkIntegrity.

@ingroup TAK_Device

@param[in] nonceLength       Expected size of nonce byte buffer.
@param[out] buffer           Byte buffer to write the nonce to.


@retval TAK_SUCCESS             Available for Android.
@retval TAK_NOT_AVAILABLE       Not available for iOS

@return The function returns the result of the transaction.
******************************************************************************/
TAK_RETURN TakLib_getSafetyNetNonce(unsigned int nonceLength, TAK_byte_buffer* buffer);

//** **************************************************************************
//** **************************************************************************

/**
 * @defgroup TAK_TLS Secure Channel
 *
 * T.A.K Secure Channel functions.
 */

/******************************************************************************
*/ /**
@brief This method returns the list of supported TLS Cipher Suites.

@ingroup TAK_TLS

@return The list of supported TLS Ciphersuites, NULL otherwise.
******************************************************************************/
char** TakLib_tlsGetSupportedCiphersuites(void);

/******************************************************************************
*/ /**
@brief This method retrieves the cipher suite being used for the connection identified by the provided socket descriptor.

@ingroup TAK_TLS

@param[in] socketDescriptor       The socket returned by TakLib_tlsConnect().
@param[out] ciphersuite       The TLS cipher suite in use in this communication.

@retval TAK_SUCCESS                 If the ciphersuite was retrieved correctly.
@retval TAK_INVALID_PARAMETER       If the provided parameters are not valid.
@retval TAK_GENERAL_ERROR           If there is a not recoverable error.

@return The function returns a value of #TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_tlsGetCiphersuite(int socketDescriptor, char **ciphersuite);

/******************************************************************************
*/ /**
@brief This method retrieves the TLS version being used for the connection identified by the provided socket descriptor.

@ingroup TAK_TLS

@param[in] socketDescriptor       The socket returned by TakLib_tlsConnect().
@param[out] tlsVersion       The TLS version in use in this communication.

@retval TAK_SUCCESS                 If the ciphersuite was retrieved correctly.
@retval TAK_INVALID_PARAMETER       If the provided parameters are not valid.
@retval TAK_GENERAL_ERROR           If there is a not recoverable error.

@return The function returns a value of #TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_tlsGetVersion(int socketDescriptor, char **tlsVersion);

/******************************************************************************
*/ /**
@brief [DEPRECATED]  Establishes a TLS socket connection to a server.

This method will set up a TLS connection (with optional client authentication)
to a server defined by its domain name/IP address and TCP port number.

A server is only trusted if its certificate matches the trustedCertificate
parameter or if a certificate chain can be established from its certificate
to the trustedCertificate parameter.

If the server requests client authentication and the T.A.K lib has registered with the T.A.K server previously, the
individual T.A.K client certificate and key will be used to authenticate.

To be used only together with TakLib_register.

\deprecated Since version 2.2.0. This method was replaced by TakLib_tlsConnectSecurePinning, use it instead.

@ingroup TAK_TLS

@param[in] fqdn        String containing a fully qualified domain name or the IP address of the server.
@param[in] port        String containing a TCP port number to connect to the server.
@param[in] trustedCertificate        String containing a certificate (in PEM format) of a trusted (Intermediate) Certificate Authority.
@param[in] timeout        Timeout in milli-seconds. A value of "0" means that network call should never timeout.
@param[out] peerCertificate        String containing the received peer certificate (if a connection could be established).
@param[out] socketDescriptor        Identifier to be used with other T.A.K TLS functions.

@retval TAK_SUCCESS                       If the TLS connection could be established successfully
@retval TAK_API_NOT_INITIALIZED           If the API was not previously initialized
@retval TAK_INVALID_PARAMETER             If the provided parameters are invalid.
@retval TAK_GENERAL_ERROR                 If general error during process.
@retval TAK_SECURITY_CERTIFICATE_ERROR    If invalid certificate or unable to use certificate.
@retval TAK_NETWORK_TIMEOUT               If error during network communication.
@retval TAK_NETWORK_ERROR                 If error during network communication.

@return The function returns a value of #TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_tlsConnect(const char *fqdn,
                             const char *port,
                             const char *trustedCertificate,
                             unsigned int timeout,
                             char **peerCertificate,
                             int *socketDescriptor) __attribute((deprecated("Use TakLib_tlsConnectSecurePinning instead")));

/******************************************************************************
*/ /**
@brief Establishes a TLS socket connection to a server.

This method will set up a TLS connection (with optional client authentication)
to a server defined by its domain name/IP address and TCP port number.

A server is only trusted if its certificate matches the trusted certificate or if a certificate chain
can be established from its certificate to the trusted certificate. The trusted certificate must be
encrypted with the T.A.K File Protector tool and attached to the application's bundle or assets.
The bundled trusted certificate must have the following style: \<fqdn>.tak (e.g.: google.com.tak).

If the server requests client authentication and the T.A.K lib has registered with the T.A.K server previously, the
individual T.A.K client certificate and key will be used to authenticate.

To be used only together with TakLib_register.

@ingroup TAK_TLS

@param[in] fqdn        String containing a fully qualified domain name or the IP address of the server.
@param[in] port        String containing a TCP port number to connect to the server.
@param[in] timeout        Timeout in milli-seconds. A value of "0" means that network call should never timeout.
@param[out] peerCertificate        String containing the received peer certificate (if a connection could be established). After TakLib_tlsConnectSecurePinning returns, peerCertificate must be freed using the dynamic memory function free(). If no free is called, a memory leak will be generated.
@param[out] socketDescriptor        Identifier to be used with other T.A.K TLS functions.

@retval TAK_SUCCESS                            If the TLS connection could be established successfully.
@retval TAK_API_NOT_INITIALIZED                If the API was not previously initialized.
@retval TAK_INVALID_PARAMETER                  If the provided parameters are invalid.
@retval TAK_GENERAL_ERROR                      If general error during process.
@retval TAK_SECURITY_CERTIFICATE_ERROR         If invalid certificate or unable to use certificate.
@retval TAK_NETWORK_TIMEOUT                    If error during network communication.
@retval TAK_NETWORK_ERROR                      If error during network communication.

@return The function returns a value of #TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_tlsConnectSecurePinning(const char *fqdn,
                                          const char *port,
                                          unsigned int timeout,
                                          char **peerCertificate,
                                          int *socketDescriptor);


/******************************************************************************
*/ /**
@brief Reads data from a server over the TLS connection established with TakLib_tlsConnect.

This method reads up to max bytes from the TLS connection identified by socketDescriptor and copies it into the buffer.
The buffer has to be initialized with length 0

@ingroup TAK_TLS

@param[in] socketDescriptor        Identifies the TLS connection.
@param[out] buffer        Buffer to store read data to.
@param[in] max        Max number of bytes to read, if is set to 0 will return all data available as TakLib_tlsReadAll.

@retval TAK_SUCCESS                  If data could be read successfully.
@retval TAK_INVALID_PARAMETER        If the provided parameters are invalid.
@retval TAK_GENERAL_ERROR            If general error during process.
@retval TAK_NETWORK_TIMEOUT          If error during network communication.

@return The function returns a value of #TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_tlsRead(int socketDescriptor, TAK_byte_buffer* buffer, int max);


/******************************************************************************
*/ /**
@brief Reads data from a server over the TLS connection established with TakLib_tlsConnect.

This method reads as many bytes from the TLS connection as available, if the server does not close the connection, a timeout will occur.
The buffer has to be initialized with length 0

@ingroup TAK_TLS

@param[in] socketDescriptor        Identifies the TLS connection.
@param[out] buffer        Buffer to store read data to.

@retval TAK_SUCCESS                  If data could be read successfully.
@retval TAK_INVALID_PARAMETER        If the provided parameters are invalid.
@retval TAK_GENERAL_ERROR            If general error during process.
@retval TAK_NETWORK_TIMEOUT          If error during network communication.

@return The function returns a value of #TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_tlsReadAll(int socketDescriptor, TAK_byte_buffer* buffer);

/******************************************************************************
*/ /**
@brief Writes data to the server over the TLS connection

This function writes writeBuffer->length bytes to the TLS connection identified by socketDescriptor.

@ingroup TAK_TLS

@param[in] socketDescriptor        Identifies the TLS connection.
@param[in] buffer         Buffer to copy data from.

@retval TAK_SUCCESS              If data could be written successfully.
@retval TAK_INVALID_PARAMETER    If the provided parameters are invalid.
@retval TAK_NETWORK_ERROR        If error during network communication.
@retval TAK_GENERAL_ERROR        If general error during process.

@return The function returns a value of #TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_tlsWrite(int socketDescriptor, TAK_byte_buffer buffer);


/******************************************************************************
*/ /**
@brief Closes the TLS connection to a server

This function closes the TLS connection and releases the corresponding handles.

@ingroup TAK_TLS

@param[in] socketDescriptor        Identifies the TLS connection

@retval TAK_SUCCESS        If the TLS connection could be closed successfully.

@return The function returns a value of #TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_tlsClose(int socketDescriptor);

/******************************************************************************
*/ /**
@brief Check if the TLS connection is still open

This function checks if the socket file descriptor is in the list of open sockets

@ingroup TAK_TLS

@param[in] socketDescriptor identifies the TLS connection

@retval true             if the TLS connection is still open
@retval false            if no corresponding socket can be found

@return The function returns true or false
******************************************************************************/
bool TakLib_tlsIsClosed(int socketDescriptor);

//** **************************************************************************
//** **************************************************************************

/**
 * @defgroup TAK_RTE Runtime Checks
 *
 * Methods for validating the runtime environment where the application is running.
 */

/******************************************************************************
*/ /**
@brief Checks if the device is rooted or jailbroken.

This function triggers root detection on Android, or the corresponding
jailbreak detection on iOS, depending on the client platform.

For more details check the section 'Root / Jailbreak detection'  of the developer documentation.

@ingroup TAK_RTE

@retval true             Device is rooted/jailbroken, is equivalent to the ROOTED level of RootStatus
@retval false            No indication that device is rooted/jailbroken could be found

@return The function returns true or false. In a remote locked status will return always false.
******************************************************************************/
bool TakLib_getRootStatus(void);

/******************************************************************************
*/ /**
@brief Checks if the device is rooted or jailbroken in a level scale.

This function triggers root detection on Android, or the corresponding
jailbreak detection on iOS, depending on the client platform.

This method is for clients that wish to handle differently each of the detected rooted or jailbroken levels,
(ex. if you need to be more restricted you could consider take action also with a maybe and likely detection levels).

Check the different levels in the TAK_ROOT_STATUS definition.

For more details check the section 'Root / Jailbreak detection'  of the developer documentation.

@ingroup TAK_RTE

@return The function returns a RootStatus Device level of root detection.
    In a remote locked status will return always NOT_ROOTED.

******************************************************************************/
TAK_ROOT_STATUS TakLib_getAdvancedRootStatus(void);

//** **************************************************************************
//** **************************************************************************

/**
 * @defgroup TAK_Storage Secure Storage
 *
 * T.A.K Secure storage functionality, to be used for storing small runtime-generated data.
 */

/******************************************************************************
*/ /**
@brief Creates a secure storage with the given name, encrypted with the symmetric key received during registration.

To be used only together with TakLib_register.

@ingroup TAK_Storage

@param[in] storageName        The name of the storage that should be created.

@retval TAK_SUCCESS                       If storage has successfully been created.
@retval TAK_API_NOT_INITIALIZED           If T.A.K has not been previously initialized.
@retval TAK_INVALID_PARAMETER             If the provided parameters are invalid.
@retval TAK_GENERAL_ERROR                 If general error during process.
@retval TAK_DEVICE_NO_FINGERPRINT         If device fingerprint could not be retrieved.
@retval TAK_STORAGE_ALREADY_EXISTS        If a storage with this name already exists.
@retval TAK_STORAGE_DEVICE_MISMATCH       App appears to be running on a different device, storage has been deleted (must be
                                          created again).
@retval TAK_INSTANCE_LOCKED               If the application has been remotely locked. This method is unavailable until the instance is unlocked.

@return The function returns a value of type TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_storageCreate(const char* storageName);

/******************************************************************************
*/ /**
@brief Write a key-value pair to a secure storage with the given name

Write a key-value pair to a secure storage with the given name. If the key already exists, the value will be overwritten.

@ingroup TAK_Storage

@param[in] storageName        The name of the storage.
@param[in] key        The name of the key under which the value gets stored in the secure storage.
@param[in] value         The value that gets stored in the secure storage.

@retval TAK_SUCCESS                    If was possible to write the key value.
@retval TAK_API_NOT_INITIALIZED        If T.A.K has not been previously initialized.
@retval TAK_INVALID_PARAMETER          The provided parameters are invalid.
@retval TAK_GENERAL_ERROR              If general error during process.
@retval TAK_STORAGE_NOT_FOUND          If a storage with this name does not exist.
@retval TAK_STORAGE_DEVICE_MISMATCH    App appears to be running on a different device, storage has been deleted (must be
                                       created again).
@retval TAK_INSTANCE_LOCKED            If the application has been remotely locked. This method is unavailable until the instance is unlocked.

@return The function returns a value of type TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_storageWrite(const char* storageName, const char* key, TAK_byte_buffer value);

/******************************************************************************
*/ /**
@brief Read a key-value pair from a secure storage

@ingroup TAK_Storage

@param[in] storageName       The name of the storage.
@param[in] key               The name of the key under which the value is stored in the secure storage.
@param[out] value            The value that was read from the secure storage. Memory must not be allocated by the user as it is done internally.

@retval TAK_SUCCESS                        If was possible to read the key value.
@retval TAK_API_NOT_INITIALIZED            If T.A.K has not been previously initialized.
@retval TAK_INVALID_PARAMETER              The provided parameters are invalid.
@retval TAK_GENERAL_ERROR                  If general error during process.
@retval TAK_STORAGE_NOT_FOUND              If a storage with this name does not exist.
@retval TAK_STORAGE_KEY_NOT_FOUND          If key identifier not found.
@retval TAK_STORAGE_DEVICE_MISMATCH        App appears to be running on a different device, storage has been deleted (must be
                                           created again).
@retval TAK_INSTANCE_LOCKED                If the application has been remotely locked. This method is unavailable until the instance is unlocked.

@return The function returns a value of type TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_storageRead(const char* storageName, const char* key, TAK_byte_buffer* value);

/******************************************************************************
*/ /**
@brief Delete a key-value pair from a secure storage with the given name

Delete a key-value pair from a secure storage with the given name, if the key does not exist, this is not treated as an
error but just ignored.

@ingroup TAK_Storage

@param[in] storageName        The name of the storage.
@param[in] key        The name of the key under which the value is stored in the secure storage.

@retval TAK_SUCCESS                        If the key has been deleted correctly.
@retval TAK_API_NOT_INITIALIZED            If T.A.K has not been previously initialized.
@retval TAK_INVALID_PARAMETER              The provided parameters are invalid.
@retval TAK_GENERAL_ERROR                  If general error during process.
@retval TAK_STORAGE_NOT_FOUND              If a storage with this name does not exist.
@retval TAK_STORAGE_KEY_NOT_FOUND          If key identifier not found.
@retval TAK_STORAGE_DEVICE_MISMATCH        App appears to be running on a different device, storage has been deleted (must be
                                           created again)
@retval TAK_INSTANCE_LOCKED                If the application has been remotely locked. This method is unavailable until the instance is unlocked.

@return The function returns a value of type TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_storageDeleteEntry(const char* storageName, const char* key);

/******************************************************************************
*/ /**
@brief Deletes a secure storage with a given name

Deletes a secure storage with a given name.

@ingroup TAK_Storage

@param[in] storageName        The name of the storage that should be deleted.

@retval TAK_SUCCESS                  If table with storeName has been deleted correctly,
@retval TAK_API_NOT_INITIALIZED      If T.A.K has not been previously initialized.
@retval TAK_INVALID_PARAMETER        The provided parameters are invalid.
@retval TAK_STORAGE_NOT_FOUND        If a storage with this name does not exist
@retval TAK_INSTANCE_LOCKED          If the application has been remotely locked. This method is unavailable until the instance is unlocked.

@return The function returns a value of type TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_storageDelete(const char* storageName);

/**
 * @defgroup FILE_PROTECTOR File Protector
 *
 * T.A.K File Protector functionality, to be used for encrypting:
 * - Compile-time data (such as API keys)
 * - Big (>1MB) runtime data
 */

/******************************************************************************
*/ /**
@brief [DEPRECATED] Decrypts a byte buffer (resource file). The function first decrypts the encrypted random AES key with the
       customer WBC key.
       The decrypted random AES key is then used to decrypt the "actual" input data (payload). The decrypted data is
       then stored in the output buffer.
    
\deprecated Since version 2.7.0. This method was replaced by TakLib_fileProtectorDecrypt, use it instead.

@ingroup FILE_PROTECTOR

@param[in] input        Encrypted input data.
@param[out] output        Decrypted output data (minimum allocated memory must be input_length - 128).

@retval TAK_SUCCESS                  If input buffer was decrypted correctly.
@retval TAK_API_NOT_INITIALIZED      If T.A.K has not been previously initialized.
@retval TAK_INVALID_PARAMETER        The provided parameters are invalid.
@retval TAK_GENERAL_ERROR            If general error during process.
@retval TAK_INSTANCE_LOCKED          This is a warning, the application has been remotely locked. Some of the functionalities will not
                                     be available until instance is unlocked.

@return The function returns a value of type TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_decryptResourceFile(TAK_byte_buffer input, TAK_byte_buffer* output) __attribute((deprecated("Use TakLib_fileProtectorDecrypt instead")));

/******************************************************************************
*/ /**
@brief Decrypts a byte buffer (resource file). The function first decrypts the encrypted random AES key with the
       customer WBC key.
       The decrypted random AES key is then used to decrypt the "actual" input data (payload). The decrypted data is
       then stored in the output buffer.

@ingroup FILE_PROTECTOR

@param[in] input        Encrypted input data.
@param[out] output        Decrypted output data (minimum allocated memory must be input_length - 128).

@retval TAK_SUCCESS                  If input buffer was decrypted correctly.
@retval TAK_API_NOT_INITIALIZED      If T.A.K has not been previously initialized.
@retval TAK_INVALID_PARAMETER        The provided parameters are invalid.
@retval TAK_GENERAL_ERROR            If general error during process.
@retval TAK_INSTANCE_LOCKED          This is a warning, the application has been remotely locked. Some of the functionalities will not
                                     be available until instance is unlocked.

@return The function returns a value of type TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_fileProtectorDecrypt(TAK_byte_buffer input, TAK_byte_buffer* output);

/******************************************************************************
*/ /**
@brief [DEPRECATED] Encrypts a byte buffer (resource file) with a random AES key in GCM mode. The random AES key gets then encrypted
       by the WBC customer key.
       The encrypted data together with the encrypted random AES key is stored in the output buffer.
    
\deprecated Since version 2.7.0. This method was replaced by TakLib_fileProtectorEncrypt, use it instead.

@ingroup FILE_PROTECTOR

@param[in] input        Input data to be encrypted.
@param[out] output        Encrypted output data (minimum allocated memory must be input_length + 128).

@retval TAK_SUCCESS                  If input buffer was encrypted correctly.
@retval TAK_API_NOT_INITIALIZED      If T.A.K has not been previously initialized.
@retval TAK_INVALID_PARAMETER        The provided parameters are invalid.
@retval TAK_GENERAL_ERROR            If general error during process.
@retval TAK_INSTANCE_LOCKED          If the application has been remotely locked. This method is unavailable until the instance is unlocked.

@return The function returns a value of type TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_encryptResourceFile(TAK_byte_buffer input, TAK_byte_buffer* output) __attribute((deprecated("Use TakLib_fileProtectorEncrypt instead")));

/******************************************************************************
*/ /**
@brief Encrypts a byte buffer (resource file) with a random AES key in GCM mode. The random AES key gets then encrypted
       by the WBC customer key.
       The encrypted data together with the encrypted random AES key is stored in the output buffer.

@ingroup FILE_PROTECTOR

@param[in] input        Input data to be encrypted.
@param[out] output        Encrypted output data (minimum allocated memory must be input_length + 128).

@retval TAK_SUCCESS                  If input buffer was encrypted correctly.
@retval TAK_API_NOT_INITIALIZED      If T.A.K has not been previously initialized.
@retval TAK_INVALID_PARAMETER        The provided parameters are invalid.
@retval TAK_GENERAL_ERROR            If general error during process.
@retval TAK_INSTANCE_LOCKED          If the application has been remotely locked. This method is unavailable until the instance is unlocked.

@return The function returns a value of type TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_fileProtectorEncrypt(TAK_byte_buffer input, TAK_byte_buffer* output);

/******************************************************************************
*/ /**
@brief Decrypts a given file (can be either name or path) in the assets.
       The decrypted data is stored in the output buffer.

@ingroup FILE_PROTECTOR

@param[in] fileName        Name or Path of the file to be decrypted
@param[in] extension    Extension of the file to be decrypted
@param[out] output        Decrypted output data.

@retval TAK_SUCCESS                  If file was found and decrypted correctly.
@retval TAK_API_NOT_INITIALIZED      If T.A.K has not been previously initialized.
@retval TAK_INVALID_PARAMETER        The provided parameters are invalid.
@retval TAK_GENERAL_ERROR            If general error during process.
@retval TAK_INSTANCE_LOCKED          This is a warning, the application has been remotely locked. Some of the functionalities will not
                                     be available until instance is unlocked.

@return The function returns a value of type TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_fileProtectorDecryptFromFile(const char* fileName, const char* extension, TAK_byte_buffer* output);

/******************************************************************************
*/ /**
@brief Returns information about a cryptographic key, identified by its alias.

Returns information about a cryptographic key, identified by its alias.

@ingroup TAK_CRYPTO

@param[in] keyAlias         Alias of the key being queried. See \ref TAK_KEY_ALIASES.
@param[out] info         Key information, containing the algorithm and the protection level.

@retval TAK_SUCCESS                    Key Information was retrieved correctly.
@retval TAK_API_NOT_INITIALIZED        If T.A.K has not been previously initialized.
@retval TAK_INVALID_PARAMETER          The provided parameters are invalid. Either the alias does not exist, or the output buffer is NULL.
@retval TAK_NOT_AVAILABLE              If client individual keys are queried but T.A.K registration has not yet happened.
@retval TAK_INSTANCE_LOCKED            If the application has been remotely locked. This method is unavailable until the instance is unlocked.
@retval TAK_NOT_REGISTERED         if the private keys for the customer do not exist or have not been generated yet.

@return The function returns a value of type TAK_RETURN
******************************************************************************/
TAK_RETURN TakLib_getKeyInfo(const char* keyAlias, TAK_KEY_INFO *info);

/******************************************************************************
*/ /**
 @brief Generate a signature by signing the input hash.

 Generate a signature by signing the input hash.

 @ingroup TAK_CRYPTO

 @param[in] keyAlias        Alias of the key. See \ref TAK_KEY_ALIASES.
 @param[in] signatureAlgorithm        Crypto algorithm to sign.
 @param[in] hashType        Type of hash function used to generate the input.
 @param[in] hashToSign        Buffer containing  the hash to sign.
 @param[out] signature        Buffer with the generated signature.

 @retval TAK_SUCCESS               If signature generation was successful.
 @retval TAK_INVALID_PARAMETER     If input parameter is invalid.
 @retval TAK_API_NOT_INITIALIZED   If TAK has not been initialized.
 @retval TAK_NOT_REGISTERED        Only for INDIVIDUAL_CLIENT alias if key is not available because register step is missing.
 @retval TAK_GENERAL_ERROR         If general error during process.
 @retval TAK_INSTANCE_LOCKED       If the application has been remotely locked. This method is unavailable until the instance is unlocked.

 @return The function returns a value of type TAK_RETURN
 ******************************************************************************/
 TAK_RETURN TakLib_sign(const char* keyAlias,
                        TAK_SIGNATURE_ALGORITHM signatureAlgorithm,
                        TAK_HASH_ALGORITHM hashType,
                        TAK_byte_buffer hashToSign,
                        TAK_byte_buffer *signature);

/******************************************************************************
*/ /**
 @brief Generate a signature by creating a hash from the input data and then signing it.

 Generate a signature by creating a hash from the input data and then signing it.

 @ingroup TAK_CRYPTO

 @param[in] keyAlias        Alias of the key. See \ref TAK_KEY_ALIASES.
 @param[in] signatureAlgorithm        Crypto algorithm to sign.
 @param[in] hashType        Type of hash function used to generate the input.
 @param[in] dataToSign        Buffer containing the data to hash and then sign.
 @param[out] signature        Buffer with the generated signature.

 @retval TAK_SUCCESS               If signature generation was successful.
 @retval TAK_INVALID_PARAMETER     If input parameter is invalid.
 @retval TAK_API_NOT_INITIALIZED   If TAK has not been initialized.
 @retval TAK_NOT_REGISTERED        Only for INDIVIDUAL_CLIENT alias if key is not available because register step is missing.
 @retval TAK_GENERAL_ERROR         If general error during process.
 @retval TAK_INSTANCE_LOCKED       If the application has been remotely locked. This method is unavailable until the instance is unlocked.

 @return The function returns a value of type TAK_RETURN
 ******************************************************************************/
 TAK_RETURN TakLib_hashAndSign(const char* keyAlias,
                               TAK_SIGNATURE_ALGORITHM signatureAlgorithm,
                               TAK_HASH_ALGORITHM hashType,
                               TAK_byte_buffer dataToSign,
                               TAK_byte_buffer *signature);

/**
 @brief Encrypts the input data using the given key and the specified algorithm.

 Encrypts the input data using the given key and the specified algorithm.

 Client-individual keys are only available after registration.

 Be careful when using client-individual encryption keys, since they may change during
 re-registration. Check the developer documentation for further details.

 Furthermore, if ENCRYPT_ALGO_AES_ECB is used, length of the clear text must be 16 bytes. However,
 note that the use of this encryption algorithm is highly discouraged. Use it only when no other
 case is available. Check the developer documentation for further details.

 @ingroup TAK_CRYPTO

 @param[in] keyAlias            Alias of the key. See \ref TAK_KEY_ALIASES.
 @param[in] encryptionAlgorithm           Algorithm to be used during operation.
 @param[in] cleartext           The input data to be encrypted.
 @param[out] encryptionOutput   Struct containing the output of the encryption. Presence of the different fields will depend on the algorithm used.

 @retval TAK_SUCCESS                If method executed successfully.
 @retval TAK_INVALID_PARAMETER      If input parameter is invalid.
 @retval TAK_API_NOT_INITIALIZED    If the API was not previously initialized.
 @retval TAK_NOT_REGISTERED         Only for INDIVIDUAL_CLIENT alias if key is not available because register step is missing.
 @retval TAK_GENERAL_ERROR          If error occurred while performing a crypto operation.
 @retval TAK_INSTANCE_LOCKED        If the application has been remotely locked. This method is unavailable until the instance is unlocked.
 */
TAK_RETURN TakLib_encrypt(const char* keyAlias,
                          TAK_ENCRYPTION_ALGORITHM encryptionAlgorithm,
                          TAK_byte_buffer cleartext,
                          TAK_ENCRYPTION_OUTPUT *encryptionOutput);

/**
 @brief Decrypts the input data using the given key and the specified algorithm.

 Decrypts the input data using the given key and the specified algorithm.

 Client-individual keys are only available after registration.

 Be careful when using client-individual encryption keys, since they may change during
 re-registration. Check the developer documentation for further details.

 @ingroup TAK_CRYPTO

 @param[in] keyAlias                  Alias of the key. See \ref TAK_KEY_ALIASES.
 @param[in] encryptionAlgorithm       Algorithm to be used during operation.
 @param[in] takPaddingType            Padding type to be used during operation. Limited to values in TAK_PADDING_TYPE.
 @param[in] ciphertext                The TAK_ENCRYPTION_OUTPUT containing the data to be decrypted.
 @param[out] cleartext                Decrypted data.

 @retval TAK_SUCCESS                If method executed successfully.
 @retval TAK_INVALID_PARAMETER      If input parameter is invalid.
 @retval TAK_API_NOT_INITIALIZED    If the API was not previously initialized.
 @retval TAK_NOT_REGISTERED         Only for INDIVIDUAL_CLIENT alias if key is not available because register step is missing.
 @retval TAK_GENERAL_ERROR          If error occurred while performing a crypto operation.
 @retval TAK_INSTANCE_LOCKED        If the application has been remotely locked. This method is unavailable until the instance is unlocked.
 */
TAK_RETURN TakLib_decryptWithPadding(const char* keyAlias,
                                 TAK_ENCRYPTION_ALGORITHM encryptionAlgorithm,
                                 TAK_PADDING_TYPE takPaddingType,
                                 TAK_ENCRYPTION_OUTPUT ciphertext,
                                 TAK_byte_buffer* cleartext);

/**
 @brief Encrypts the input data using the given key and the specified algorithm.

 Encrypts the input data using the given key and the specified algorithm.

 Client-individual keys are only available after registration.

 Be careful when using client-individual encryption keys, since they may change during
 re-registration. Check the developer documentation for further details.

 Furthermore, if ENCRYPT_ALGO_AES_ECB is used, length of the clear text must be 16 bytes. However,
 note that the use of this encryption algorithm is highly discouraged. Use it only when no other
 case is available. Check the developer documentation for further details.

 Lastly, this method permits the use of PKCS5 padding only with AES-CBC. Before using it, please refer
 to the developer documentation for more information as applying PKCS padding in a CBC encryption can create unwanted vulnerabilities.

 @ingroup TAK_CRYPTO

 @param[in] keyAlias                  Alias of the key. See \ref TAK_KEY_ALIASES.
 @param[in] encryptionAlgorithm       Algorithm to be used during operation.
 @param[in] takPaddingType            Padding type to be used during operation. Limited to values in TAK_PADDING_TYPE.
 @param[in] cleartext                 The input data to be encrypted.
 @param[in] iv                        The initialization vector to be used, it is required. If length is 0 or HwBacked cryptography is used the IV will be generated randomly.
 @param[out] encryptionOutput         Struct containing the output of the encryption. Presence of the different fields will depend on the algorithm used.

 @retval TAK_SUCCESS                If method executed successfully.
 @retval TAK_INVALID_PARAMETER      If input parameter is invalid.
 @retval TAK_API_NOT_INITIALIZED    If the API was not previously initialized.
 @retval TAK_NOT_REGISTERED         Only for INDIVIDUAL_CLIENT alias if key is not available because register step is missing.
 @retval TAK_GENERAL_ERROR          If error occurred while performing a crypto operation.
 @retval TAK_INSTANCE_LOCKED        If the application has been remotely locked. This method is unavailable until the instance is unlocked.
 */
TAK_RETURN TakLib_encryptWithPadding(const char* keyAlias,
                                 TAK_ENCRYPTION_ALGORITHM encryptionAlgorithm,
                                 TAK_PADDING_TYPE takPaddingType,
                                 TAK_byte_buffer cleartext,
                                 TAK_byte_buffer iv,
                                 TAK_ENCRYPTION_OUTPUT *encryptionOutput);

/**
 @brief Decrypts the input data using the given key and the specified algorithm.

 Decrypts the input data using the given key and the specified algorithm.

 Client-individual keys are only available after registration.

 Be careful when using client-individual encryption keys, since they may change during
 re-registration. Check the developer documentation for further details.

 @ingroup TAK_CRYPTO

 @param[in] keyAlias                  Alias of the key. See \ref TAK_KEY_ALIASES.
 @param[in] encryptionAlgorithm       Algorithm to be used during operation.
 @param[in] ciphertext                The TAK_ENCRYPTION_OUTPUT containing the data to be decrypted.
 @param[out] cleartext                Decrypted data.

 @retval TAK_SUCCESS                If method executed successfully.
 @retval TAK_INVALID_PARAMETER      If input parameter is invalid.
 @retval TAK_API_NOT_INITIALIZED    If the API was not previously initialized.
 @retval TAK_NOT_REGISTERED         Only for INDIVIDUAL_CLIENT alias if key is not available because register step is missing.
 @retval TAK_GENERAL_ERROR          If error occurred while performing a crypto operation.
 @retval TAK_INSTANCE_LOCKED        If the application has been remotely locked. This method is unavailable until the instance is unlocked.
 */
TAK_RETURN TakLib_decrypt(const char* keyAlias,
                          TAK_ENCRYPTION_ALGORITHM encryptionAlgorithm,
                          TAK_ENCRYPTION_OUTPUT ciphertext,
                          TAK_byte_buffer* cleartext);

/**
 @brief Random Generator.

 Generates a random byte array of the requested size.
 
 Device must be registered before using this API.

 @ingroup TAK_CRYPTO

 @param[in] numBytes            Size in bytes of the amount of random data to be generated.
 @param[out] randomData     Randomly generated byte array of size numBytes. The array's data must not be previously allocated.

 @retval TAK_SUCCESS                    If method executed successfully.
 @retval TAK_INVALID_PARAMETER          If input parameter is invalid.
 @retval TAK_API_NOT_INITIALIZED        If the API was not previously initialized.
 @retval TAK_NOT_REGISTERED             If key is not available because register step is missing.
 @retval TAK_GENERAL_ERROR              If general error during process.
 @retval TAK_INVALID_SERVER_RESPONSE    If the server response was not as expected.
 @retval TAK_NOT_AVAILABLE              If the server response 404 or cert has expired.
 @retval TAK_NETWORK_TIMEOUT            If error during network communication.
 @retval TAK_NETWORK_ERROR              If error during network communication.
 @retval TAK_SECURITY_CERTIFICATE_ERROR If invalid certificate or unable to use certificate.
 @retval TAK_INSTANCE_LOCKED            If the application has been remotely locked. This method is unavailable until the instance is unlocked.
 */
TAK_RETURN TakLib_generateRandom(int numBytes, TAK_byte_buffer* randomData);

/**
 @brief Load key from user, this key is in wrap format.
 
 Unwrap and stores input key in order to be used.

 @ingroup TAK_CRYPTO

 @param[in] newKeyWrapped       Key wrapped format.
 @param[in] newKeyAlias         Alias for the new key in order to be stored.

 @retval TAK_SUCCESS                     If method executed successfully.
 @retval TAK_INVALID_PARAMETER           If input parameter is invalid.
 @retval TAK_API_NOT_INITIALIZED         If the API was not previously initialized.
 @retval TAK_NOT_REGISTERED              If key is not available because register step is missing.
 @retval TAK_GENERAL_ERROR               If general error during process.
 @retval TAK_INVALID_SERVER_RESPONSE     If the server response was not as expected.
 @retval TAK_NOT_AVAILABLE               If the server response 404 or cert has expired.
 @retval TAK_NETWORK_TIMEOUT             If error during network communication.
 @retval TAK_NETWORK_ERROR               If error during network communication.
 @retval TAK_SECURITY_CERTIFICATE_ERROR  If invalid certificate or unable to use certificate.
 @retval TAK_INSTANCE_LOCKED             If the application has been remotely locked. This method is unavailable until the instance is unlocked.
 */
TAK_RETURN TakLib_loadKey(TAK_WRAPPED_KEY newKeyWrapped, const char* newKeyAlias);

/**
 @brief Load a RSA_2048 key from the user, this key is in wrap format padded with PKCS5.

 Unwrap and stores input key in order to be used.

 @ingroup TAK_CRYPTO

 @param[in] newKeyWrapped       Key wrapped format.
 @param[in] newKeyAlias         Alias for the new key in order to be stored.

 @retval TAK_SUCCESS                     If method executed successfully.
 @retval TAK_INVALID_PARAMETER           If input parameter is invalid.
 @retval TAK_API_NOT_INITIALIZED         If the API was not previously initialized.
 @retval TAK_NOT_REGISTERED              If key is not available because register step is missing.
 @retval TAK_GENERAL_ERROR               If general error during process.
 @retval TAK_INVALID_SERVER_RESPONSE     If the server response was not as expected.
 @retval TAK_NOT_AVAILABLE               If the server response 404 or cert has expired.
 @retval TAK_NETWORK_TIMEOUT             If error during network communication.
 @retval TAK_NETWORK_ERROR               If error during network communication.
 @retval TAK_SECURITY_CERTIFICATE_ERROR  If invalid certificate or unable to use certificate.
 @retval TAK_INSTANCE_LOCKED             If the application has been remotely locked. This method is unavailable until the instance is unlocked.
 */
TAK_RETURN TakLib_loadKeyWithPKCS5(TAK_WRAPPED_KEY newKeyWrapped, const char* newKeyAlias) __attribute((deprecated("Use TakLib_loadKey whenever possible.")));

/**
 @brief Configures a pre-crash callback.
 
 This method configures a pre-crash callback that will be called before crashing the application.

 @warning   Using this method may suppose a security risk.
            T.A.K will execute the callback and then come back to T.A.K to crash the process,
            which can be used by an attacker to prevent the crash.
            Check the Developer Documentation for some recommendations on how to mitigate the risk.

 @ingroup TAK_RTE

 @param[in] callback    Function callback that will be called before crashing the application.

 @retval TAK_SUCCESS            if method executed successfully.
 @retval TAK_INVALID_PARAMETER  if callback is NULL.
 */
TAK_RETURN TakLib_setPreCrashCallback(void (*callback)(TAK_RUNTIME_CRASH_REASON));

/**
 @brief Creates a background thread hat launches runtime checks.

 This method creates a thread that will launch runtime checks periodically on background.

 @ingroup TAK_RUNTIME

 @param timeInterval                     Interval in seconds to repeat the thread's execution.
                                         The minimum value for it is 45. The recommended amount of seconds is
                                         also 45 seconds, but if performance is key then it is recommended to
                                         increase it to 60 seconds.

 @retval TAK_SUCCESS                  If the thread has been created successfully.
 @retval TAK_GENERAL_ERROR            If the process of creating the thread fails unexpectedly.
 @retval TAK_INVALID_PARAMETER        If the amount of seconds is below the minimum or it is NULL.
 @retval TAK_API_NOT_INITIALIZED      If the TAK API has not been initialized previously.
 */
TAK_RETURN TakLib_createRuntimeCheckThread(int timeInterval);

/**
 @brief Stops runtime checks' thread if running.

 This method stops the thread that periodically executes the runtime checks if it is running.

 @ingroup TAK_RUNTIME

 @returnval TAK_SUCCESS                  If the thread has been stopped or was previously stopped.
 @returnval TAK_GENERAL_ERROR            If the process of stopping the thread fails unexpectedly.
 @returnval TAK_API_NOT_INITIALIZED      If the TAK API has not been initialized previously.

 */
TAK_RETURN TakLib_stopRuntimeThread(void);

/**
 @brief Checks if runtime checks' thread is running.

 This method will check if the runtime checks' thread is running. In case that it should be running,
 additionally, it will relaunch the runtime checks' thread.

 @ingroup TAK_RUNTIME

 @retval true                         If the thread is running or has been relaunched.
 @retval false                        If the thread is expected not to be running.
 */
bool TakLib_isRuntimeThreadActive(bool relaunch);

/**
 @brief Generate and store a single symmetric key or an asymmetric pair of keys.
 
 This method generates a symmetric key and stores it in the secure storage with a specified alias for its use, or
 generates a pair of asymmetric pair of keys, storing the private and public keys with the specified alias,
 allowing for its retrieveal through the TakLib_getPublicKey method.
 
 Does not accept as keyAlias the pre-defined Default and Individual key aliases.
 
 @ingroup TAK_Device
 
 @param[in] keyAlias       Alias by which the key or key pair will be known.
 @param[in] keyAlgorithm         The cryptographic algorithm of the key or keys.
 
 @retval TAK_SUCCESS                    If the key was successfully generated and stored.
 @retval TAK_INVALID_SERVER_RESPONSE                    If there was a problem with the returned data from the server.
 @retval TAK_INVALID_PARAMETER                  If there was an issue with any of the input parameters.
 @retval TAK_API_NOT_INITIALIZED                    If the TAK API has not been initialized previously.
 @retval TAK_NETWORK_TIMEOUT                    If error during network communication.
 @retval TAK_NETWORK_ERROR                  If error during network communication.
 @retval TAK_NOT_REGISTERED                 If key is not available because register step is missing.
 @retval TAK_INSTANCE_LOCKED                    If the application has been remotely locked. This method is unavailable until the instance is unlocked.
 @retval TAK_NOT_AVAILABLE                  If the server response 404 or cert has expired.
 */
TAK_RETURN TakLib_keyGenerator(const char* keyAlias, TAK_KEY_ALGORITHM keyAlgorithm);

/**
 @brief Retrieve a public key given an alias.
 
 This method returns a previously generated public key of an asymmetric algorithm, generated with TakLib_keyGenerator.
 
 Do not allocate memory to the publicKey parameter to avoid memory leaks.
 Free the memory of the publicKey parameter after its use to avoid memory leaks.
 
 @ingroup TAK_Device
 
 @param[in] keyAlias       Alias assigned to the key pair during the generation process.
 @param[out] publicKey         Byte Buffer structure where the Public Key will be returned.
 
 @retval TAK_SUCCESS                    If the public key was successfully retrieved.
 @retval TAK_INVALID_PARAMETER                  If there was an issue with any of the input parameters.
 @retval TAK_API_NOT_INITIALIZED                    If the TAK API has not been initialized previously.
 @retval TAK_NOT_REGISTERED                 If key is not available because register step is missing.
 @retval TAK_INSTANCE_LOCKED                    If the application has been remotely locked. This method is unavailable until the instance is unlocked.
 @retval TAK_NOT_AVAILABLE                  If the server response 404 or cert has expired.
 */
TAK_RETURN TakLib_getPublicKey(const char* keyAlias, TAK_byte_buffer* publicKey);

#ifdef TARGET_ANDROID
/**
 * @brief Block screen capture in Android applications. The method requires running on the UI Thread.
 * Use activity.runOnUiThread when calling it.
 *
 * This method blocks the screen from being captured in the given activity.
 *
 * @ingroup SCREEN_PROTECTOR
 *
 * @param[in] activity                  jobject structure pointer of the activity
 *
 * @retval TAK_SUCCESS                  If the process of screen capture prevention was completed successfully.
 * @retval TAK_INVALID_PARAMETER        If there was an issue validating the input parameter.
 * @retval TAK_API_NOT_INITIALIZED      If the TAK API has not been initialized previously.
 * @retval TAK_INSTANCE_LOCKED          If the application has been remotely locked. This method is unavailable until the instance is unlocked.
 * @retval TAK_INVALID_EXECUTION_THREAD If the function was not ran on the UI Thread. Please use activity.runOnUiThread.
 * @retval TAK_GENERAL_ERROR            If a generic error, such as an issue when thread locking, occurs.
 */
TAK_RETURN TakLib_blockScreenCapture(jobject* activity);
#endif /* TARGET_ANDROID */

#ifdef __cplusplus
}
#endif

#endif
