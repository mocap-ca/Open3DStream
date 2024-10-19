//
// Created by matin on 2024-10-13.
//

#ifndef AUDIO_H
#define AUDIO_H

#include <Audioclient.h>
#include <fbsdk/fbsdk.h>
#include <vector>

struct IMMDevice;
struct IAudioClient;
struct IAudioRenderClient;
namespace std {
    class thread;
}

class IAudioSubscriber {
public:
    virtual ~IAudioSubscriber() = default;
    virtual void audio_captured(const BYTE* captureBuffer, UINT32 nFrames) = 0;
};

class Open3D_AudioInput
{
public:
    Open3D_AudioInput();
    ~Open3D_AudioInput();
    void set_device(FBAudioIn* mic);
    void subscribe(IAudioSubscriber* subscriber) { subscribers.push_back(subscriber); }
    void unsubscribe(IAudioSubscriber* subscriber) { subscribers.erase(std::remove(subscribers.begin(), subscribers.end(), subscriber), subscribers.end()); }

private:
    // Audio Device Activation Section
    // ===============================
    
    IMMDevice* recorder = nullptr;
    IAudioClient* recorderClient = nullptr;
    IAudioCaptureClient* recorderService = nullptr;

    WAVEFORMATEX* format = nullptr;
    PROPERTYKEY mNameKey = {};

    void activate();
    void release();

    // Audio Capture Loop Section
    // ==========================

    BYTE* captureBuffer = nullptr;
    UINT32 nFrames = 0;
    DWORD flags = 0;

    std::thread* recordThread = nullptr;
    bool stopThread = false;

    void loop();

    // Audio Capture Publishing to Subscribers Section
    // ===============================================

    std::vector<IAudioSubscriber*> subscribers;
    void publish_audio_captured() const;
};



#endif //AUDIO_H
