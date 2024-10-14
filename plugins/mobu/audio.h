//
// Created by matin on 2024-10-13.
//

#ifndef AUDIO_H
#define AUDIO_H

#include <Audioclient.h>
#include <fbsdk/fbsdk.h>

struct IMMDevice;
struct IAudioClient;
struct IAudioRenderClient;

class Open3D_AudioInput
{
public:
    Open3D_AudioInput();
    ~Open3D_AudioInput();
    void set_device(FBAudioIn* mic);

private:
    IMMDevice* recorder = nullptr;
    IAudioClient* recorderClient = nullptr;
    IAudioRenderClient* renderService = nullptr;
    WAVEFORMATEX* format = nullptr;
    PROPERTYKEY mNameKey;

    void recorder_release();
};



#endif //AUDIO_H
