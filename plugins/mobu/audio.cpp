//
// Created by matin on 2024-10-13.
//

#include "audio.h"

#include <mmdeviceapi.h>

#include <locale>
#include <system_error>

// Throw a std::system_error if the HRESULT indicates failure.
template<typename T>
void ThrowIfFailed(HRESULT hr, T&& msg) {
    if( FAILED( hr ) ) {
        throw std::system_error{hr, std::system_category(), std::forward<T>(msg)};
    }
}

int WideCompare(const LPWSTR wstr, const char* str)
{
    wchar_t ws[255];
    swprintf(ws, 255, L"%hs", str);
    return wcscmp(wstr, ws);
}

Open3D_AudioInput::Open3D_AudioInput() {
    //auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    //ThrowIfFailed(hr, "Failed to initialize the MMDeviceAPI.");

    GUID IDevice_FriendlyName = {
        0xa45c254e, 0xdf1c, 0x4efd,
        { 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0 }
    };
    mNameKey.pid = 14;
    mNameKey.fmtid = IDevice_FriendlyName;
}

void Open3D_AudioInput::recorder_release() {
    if (recorder) {
        recorder->Release();
        recorder = nullptr;
    }
}

Open3D_AudioInput::~Open3D_AudioInput() {
    recorder_release();
    //CoUninitialize();
}

void Open3D_AudioInput::set_device(FBAudioIn *mic) {
    recorder_release();
    if (mic == nullptr) {
        return;
    }

    IMMDeviceEnumerator* enumerator = nullptr;
    IMMDeviceCollection* recorderCollection = nullptr;
    IMMDevice* device = nullptr;
    IPropertyStore* properties = nullptr;
    UINT count;

    HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator), reinterpret_cast<void **>(&enumerator)
    );
    ThrowIfFailed(hr, "Failed to create an instance of MMDeviceEnumerator.");

    hr = enumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &recorderCollection);
    ThrowIfFailed(hr, "Failed to create an instance of MMDeviceEnumerator.");

    hr = recorderCollection->GetCount(&count);
    ThrowIfFailed(hr, "Failed to get MMDevice counts.");

    for (UINT i=0; i<count; ++i) {
        hr = recorderCollection->Item(i, &device);
        ThrowIfFailed(hr, "Failed to get MMDevice.");

        hr = device->OpenPropertyStore(STGM_READ, &properties);
        ThrowIfFailed(hr, "Failed to open property store for the MMDevice.");

        PROPVARIANT varName;
        PropVariantInit(&varName);
        hr = properties->GetValue(mNameKey, &varName);
        ThrowIfFailed(hr, "Failed to get property value for the MMDevice.");

        if (varName.vt != VT_EMPTY) {
            if (WideCompare(varName.pwszVal, mic->Name.AsString()) == 0) {
                recorder = device;
                properties->Release();
                break;
            }
        }

        properties->Release();
        device->Release();
    }
    recorderCollection->Release();
    enumerator->Release();
}
