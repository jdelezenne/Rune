#pragma once

#include <webgpu/webgpu.h>

#ifdef __cplusplus
extern "C" {
#endif

void rune_webgpu_request_adapter(
    WGPUInstance instance,
    WGPURequestAdapterCallback callback,
    void* userdata1,
    void* userdata2);

void rune_webgpu_request_device(
    WGPUAdapter adapter,
    WGPURequestDeviceCallback callback,
    void* userdata1,
    void* userdata2);

#ifdef __cplusplus
}
#endif
