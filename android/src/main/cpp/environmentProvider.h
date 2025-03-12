
#ifndef ENVIRONMENT_PROVIDER_HEADER

#define ENVIRONMENT_PROVIDER_HEADER

extern "C" {
    void getEnvironment(void** environment);
    void getContext(void* context);
    void releaseEnvironment(void);
}
#endif // ENVIRONMENT_PROVIDER_HEADER