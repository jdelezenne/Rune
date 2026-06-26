#include "WebGPUAdapter.h"

void rune_webgpu_request_adapter(
    WGPUInstance instance,
    WGPURequestAdapterCallback callback,
    void* userdata1,
    void* userdata2)
{
    wgpuInstanceRequestAdapter(
        instance,
        NULL,
        (WGPURequestAdapterCallbackInfo){
            .mode = WGPUCallbackMode_AllowSpontaneous,
            .callback = callback,
            .userdata1 = userdata1,
            .userdata2 = userdata2,
        });
}

void rune_webgpu_request_device(
    WGPUAdapter adapter,
    WGPURequestDeviceCallback callback,
    void* userdata1,
    void* userdata2)
{
    wgpuAdapterRequestDevice(
        adapter,
        NULL,
        (WGPURequestDeviceCallbackInfo){
            .mode = WGPUCallbackMode_AllowSpontaneous,
            .callback = callback,
            .userdata1 = userdata1,
            .userdata2 = userdata2,
        });
}
