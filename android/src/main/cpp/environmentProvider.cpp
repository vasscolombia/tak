#include "environmentProvider.h"

#include <jni.h>

extern "C" {
    JavaVM* gJavaVM;
    jobject gContext = NULL;

    jint JNI_OnLoad(JavaVM* vm, void* reserved) {
        gJavaVM = vm;
        return JNI_VERSION_1_6;
    }

    JNIEXPORT jint JNICALL
    Java_com_build38_tak_flutter_TakPlugin_loadEnvironment(JNIEnv *env, jobject thiz, jobject context) {
        gContext = env->NewGlobalRef(context);
        return 0;
    }

    void getEnvironment(void** environment) {
        (*gJavaVM).AttachCurrentThread((JNIEnv **) environment, NULL);
    }

    void getContext(void* context) {
        *((jobject*) context) = gContext;
    }

    void releaseEnvironment() {
        JNIEnv* jniEnvironment = NULL;
        (*gJavaVM).AttachCurrentThread(&jniEnvironment, NULL);
        (*jniEnvironment).DeleteGlobalRef(gContext);
        gJavaVM = NULL;
    }
}