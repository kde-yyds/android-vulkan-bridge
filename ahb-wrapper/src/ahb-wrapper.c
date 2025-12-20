/*
 * Simple AHardwareBuffer wrapper for libhybris
 * Compile with:
 * gcc -shared -fPIC -o libahb_wrapper.so ahb_wrapper.c -I/usr/include/hybris -lhybris_common
 */

#include <dlfcn/dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Define constants if they're not already defined */
#ifndef RTLD_LAZY
#define RTLD_LAZY 1
#endif

// Define AHardwareBuffer struct if it's not already defined
typedef struct AHardwareBuffer AHardwareBuffer;
typedef struct AHardwareBuffer_Desc AHardwareBuffer_Desc;
typedef struct native_handle native_handle_t;

// Function pointer types
typedef void (*pfnAHardwareBuffer_release)(AHardwareBuffer*);
typedef int (*pfnAHardwareBuffer_sendHandleToUnixSocket)(const AHardwareBuffer*, int);
typedef int (*pfnAHardwareBuffer_allocate)(const AHardwareBuffer_Desc*, AHardwareBuffer**);
typedef const native_handle_t* (*pfnAHardwareBuffer_getNativeHandle)(const AHardwareBuffer*);

// Global function pointers
static pfnAHardwareBuffer_release _AHardwareBuffer_release = NULL;
static pfnAHardwareBuffer_sendHandleToUnixSocket _AHardwareBuffer_sendHandleToUnixSocket = NULL;
static pfnAHardwareBuffer_allocate _AHardwareBuffer_allocate = NULL;
static pfnAHardwareBuffer_getNativeHandle _AHardwareBuffer_getNativeHandle = NULL;
static int _initialized = 0;

// Initialize the wrapper
static int init_ahb_wrapper() {
    if (_initialized) return 1;
    
    // Use hybris's dlopen to access Android's libandroid.so
    void* android_handle = hybris_dlopen("libandroid.so", RTLD_LAZY);
    if (!android_handle) {
        fprintf(stderr, "Failed to load libandroid.so through hybris: %s\n", hybris_dlerror());
        return 0;
    }
    
    _AHardwareBuffer_release = (pfnAHardwareBuffer_release)hybris_dlsym(android_handle, "AHardwareBuffer_release");
    if (!_AHardwareBuffer_release) {
        fprintf(stderr, "Failed to find AHardwareBuffer_release: %s\n", hybris_dlerror());
        hybris_dlclose(android_handle);
        return 0;
    }
    
    _AHardwareBuffer_sendHandleToUnixSocket = 
        (pfnAHardwareBuffer_sendHandleToUnixSocket)hybris_dlsym(android_handle, "AHardwareBuffer_sendHandleToUnixSocket");
    if (!_AHardwareBuffer_sendHandleToUnixSocket) {
        fprintf(stderr, "Failed to find AHardwareBuffer_sendHandleToUnixSocket: %s\n", hybris_dlerror());
        hybris_dlclose(android_handle);
        return 0;
    }

    _AHardwareBuffer_allocate = (pfnAHardwareBuffer_allocate)hybris_dlsym(android_handle, "AHardwareBuffer_allocate");
    if (!_AHardwareBuffer_allocate) {
        fprintf(stderr, "Failed to find AHardwareBuffer_allocate: %s\n", hybris_dlerror());
        hybris_dlclose(android_handle);
        return 0;
    }

    _AHardwareBuffer_getNativeHandle = (pfnAHardwareBuffer_getNativeHandle)hybris_dlsym(android_handle, "AHardwareBuffer_getNativeHandle");
    if (!_AHardwareBuffer_getNativeHandle) {
        fprintf(stderr, "Failed to find AHardwareBuffer_getNativeHandle: %s\n", hybris_dlerror());
        hybris_dlclose(android_handle);
        return 0;
    }
    
    fprintf(stderr, "AHardwareBuffer functions successfully loaded\n");
    _initialized = 1;
    return 1;
}

// Wrapper for AHardwareBuffer_release
void AHardwareBuffer_release(AHardwareBuffer* buffer) {
    if (!_initialized && !init_ahb_wrapper()) {
        fprintf(stderr, "AHB wrapper not initialized\n");
        return;
    }
    
    _AHardwareBuffer_release(buffer);
}

// Wrapper for AHardwareBuffer_sendHandleToUnixSocket
int AHardwareBuffer_sendHandleToUnixSocket(const AHardwareBuffer* buffer, int socket) {
    if (!_initialized && !init_ahb_wrapper()) {
        fprintf(stderr, "AHB wrapper not initialized\n");
        return -1;
    }
    
    return _AHardwareBuffer_sendHandleToUnixSocket(buffer, socket);
}

// Wrapper for AHardwareBuffer_allocate
int AHardwareBuffer_allocate(const AHardwareBuffer_Desc* desc, AHardwareBuffer** outBuffer) {
    if (!_initialized && !init_ahb_wrapper()) {
        fprintf(stderr, "AHB wrapper not initialized\n");
        return -1;
    }

    return _AHardwareBuffer_allocate(desc, outBuffer);
}

// Wrapper for AHardwareBuffer_getNativeHandle
const native_handle_t* AHardwareBuffer_getNativeHandle(const AHardwareBuffer* buffer) {
    if (!_initialized && !init_ahb_wrapper()) {
        fprintf(stderr, "AHB wrapper not initialized\n");
        return NULL;
    }

    return _AHardwareBuffer_getNativeHandle(buffer);
}
