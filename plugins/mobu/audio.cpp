//
// Created by matin on 2024-10-13.
//

#include "audio.h"

#include <mmdeviceapi.h>

#include <locale>
#include <system_error>
#include <thread>

constexpr int REFTIMES_PER_SEC = 10000000;

// Throw a std::system_error if the HRESULT indicates failure.
template<typename T>
void ThrowIfFailed(HRESULT hr, T&& msg) {
    if( FAILED( hr ) ) {
        throw std::system_error{hr, std::system_category(), std::forward<T>(msg)};
    }
}

int WideCompare(LPWSTR wstr, const char* str)
{
    wchar_t ws[255];
    swprintf(ws, 255, L"%hs", str);
    return wcscmp(wstr, ws);
}

Open3D_AudioInput::Open3D_AudioInput() {
    //auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    //ThrowIfFailed(hr, "Failed to initialize the MMDeviceAPI.");

    mNameKey.pid = 14;
    mNameKey.fmtid = {
        0xa45c254e, 0xdf1c, 0x4efd,
        { 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0 }
    };
}

void Open3D_AudioInput::publish_audio_captured() const {
    for (const auto subscriber : subscribers) {
        subscriber->audio_captured(captureBuffer, nFrames);
    }
}

void Open3D_AudioInput::activate() {
    if (recorder == nullptr) {
        return;
    }

    auto hr = recorder->Activate(__uuidof(IAudioClient), CLSCTX_ALL,
                                         nullptr, reinterpret_cast<void**>(&recorderClient));
    ThrowIfFailed(hr, "Failed to activate the recorder.");

    hr = recorderClient->GetMixFormat(&format);
    ThrowIfFailed(hr, "Failed to get the recording format.");

    hr = recorderClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, REFTIMES_PER_SEC,
                                    0, format, nullptr);
    ThrowIfFailed(hr, "Failed to initialize the recording client with the recording format.");

    hr = recorderClient->GetService(__uuidof(IAudioCaptureClient), reinterpret_cast<void**>(&recorderService));
    ThrowIfFailed(hr, "Failed to get the capture service for the recorder client.");

    hr = recorderClient->Start();
    ThrowIfFailed(hr, "Failed to start the recorder.");

    recordThread = new std::thread(&Open3D_AudioInput::loop, this);
}

void Open3D_AudioInput::release() {
    if (recordThread) {
        stopThread = true;
        recordThread->join();
        delete recordThread;
        recordThread = nullptr;
        stopThread = false;
    }
    if (recorderClient) {
        recorderClient->Stop();

        if (recorderService) {
            recorderService->Release();
            recorderService = nullptr;
        }
        recorderClient->Release();
        recorderClient = nullptr;
    }
    if (recorder) {
        recorder->Release();
        recorder = nullptr;
    }
}

void Open3D_AudioInput::loop() {
    HRESULT hr;
    while (!stopThread) {
        hr = recorderService->GetBuffer(&captureBuffer, &nFrames, &flags, nullptr, nullptr);
        ThrowIfFailed(hr, "Failed to get recording buffer.");

        if (captureBuffer == nullptr)
            continue;

        hr = recorderService->ReleaseBuffer(nFrames);
        ThrowIfFailed(hr, "Failed to release recording buffer");        

        publish_audio_captured();
    }
}

Open3D_AudioInput::~Open3D_AudioInput() {
    release();
    //CoUninitialize();
}

void Open3D_AudioInput::set_device(FBAudioIn *mic) {
    release();
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

    activate();
}
