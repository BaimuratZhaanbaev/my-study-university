#include <windows.h>
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <conio.h>

// Подключение библиотеки XAudio2
#pragma comment(lib, "xaudio2.lib")

struct WAVHeader {
    uint32_t riff;
    uint32_t size0;
    uint32_t wave;
    uint32_t fmt;
    uint32_t fmtSize;
    uint16_t audioFormat;
    uint16_t channels;
    uint32_t sampleRate;
    uint32_t avgBytesPerSec;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    uint32_t data;
    uint32_t dataSize;
};

HRESULT LoadWAV(const std::string& filename, WAVEFORMATEX& wfx, std::vector<uint8_t>& wavData) {
    WAVHeader header = {};
    std::ifstream file(filename, std::ios::binary);
    if (!file) return E_FAIL;

    file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
    if (!file) return E_FAIL;

    wavData.resize(header.dataSize);
    file.read(reinterpret_cast<char*>(wavData.data()), header.dataSize);
    if (!file) return E_FAIL;

    wfx.wFormatTag = 1;
    wfx.nChannels = header.channels;
    wfx.nSamplesPerSec = header.sampleRate;
    wfx.wBitsPerSample = header.bitsPerSample;
    wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    return S_OK;
}

int main() {
    HRESULT hr = S_OK;
    IXAudio2* pXAudio2 = nullptr;
    IXAudio2MasteringVoice* pMasterVoice = nullptr;
    IXAudio2SourceVoice* pSourceVoice1 = nullptr;
    IXAudio2SourceVoice* pSourceVoice2 = nullptr;
    IUnknown* pReverb = nullptr;

    hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) return -1;

    hr = XAudio2Create(&pXAudio2, 0);
    if (FAILED(hr)) { CoUninitialize(); return -1; }

    hr = pXAudio2->CreateMasteringVoice(&pMasterVoice);
    if (FAILED(hr)) { pXAudio2->Release(); CoUninitialize(); return -1; }

    // Ввод файлов
    std::string filename1, filename2;
    std::cout << "Enter path to first WAV file: ";
    std::getline(std::cin, filename1);
    std::cout << "Enter path to second WAV file (or empty for single): ";
    std::getline(std::cin, filename2);

    // Ввод pitch
    float pitch = 1.0f;
    std::cout << "Enter pitch ratio (e.g., 1.5 for higher, 0.5 for lower): ";
    std::cin >> pitch;

    // Загрузка первого файла
    WAVEFORMATEX wfx1 = {};
    std::vector<uint8_t> wavData1;
    hr = LoadWAV(filename1, wfx1, wavData1);
    if (FAILED(hr)) { /* cleanup */ return -1; }

    // Создание source voice 1
    hr = pXAudio2->CreateSourceVoice(&pSourceVoice1, &wfx1);
    if (FAILED(hr)) { /* cleanup */ return -1; }

    // Применение pitch
    pSourceVoice1->SetFrequencyRatio(pitch);

    // Создание reverb эффекта для эхо
    hr = XAudio2CreateReverb(&pReverb);
    if (FAILED(hr)) { /* cleanup */ return -1; }

    XAUDIO2_EFFECT_DESCRIPTOR effects[] = { {pReverb, TRUE, 1} };
    XAUDIO2_EFFECT_CHAIN effectChain = { 1, effects };
    hr = pSourceVoice1->SetEffectChain(&effectChain);
    if (FAILED(hr)) { /* cleanup */ return -1; }

    // Параметры reverb для эхо (средний room)
    XAUDIO2FX_REVERB_PARAMETERS reverbParams = {};
    reverbParams.WetDryMix = 50.0f;
    reverbParams.RoomSize = 80;
    reverbParams.ReverbTime = 2.0f;
    hr = pSourceVoice1->SetEffectParameters(0, &reverbParams, sizeof(reverbParams));
    if (FAILED(hr)) { /* cleanup */ return -1; }

    // Submit buffer 1
    XAUDIO2_BUFFER buffer1 = { 0 };
    buffer1.AudioBytes = static_cast<UINT32>(wavData1.size());
    buffer1.pAudioData = wavData1.data();
    buffer1.Flags = XAUDIO2_END_OF_STREAM;
    pSourceVoice1->SubmitSourceBuffer(&buffer1);

    // Второй файл, если указан
    if (!filename2.empty()) {
        WAVEFORMATEX wfx2 = {};
        std::vector<uint8_t> wavData2;
        hr = LoadWAV(filename2, wfx2, wavData2);
        if (FAILED(hr)) { /* cleanup */ return -1; }

        hr = pXAudio2->CreateSourceVoice(&pSourceVoice2, &wfx2);
        if (FAILED(hr)) { /* cleanup */ return -1; }

        pSourceVoice2->SetFrequencyRatio(pitch); // Тот же pitch

        // Submit buffer 2
        XAUDIO2_BUFFER buffer2 = { 0 };
        buffer2.AudioBytes = static_cast<UINT32>(wavData2.size());
        buffer2.pAudioData = wavData2.data();
        buffer2.Flags = XAUDIO2_END_OF_STREAM;
        pSourceVoice2->SubmitSourceBuffer(&buffer2);
    }

    // Запуск
    pSourceVoice1->Start(0);
    if (pSourceVoice2) pSourceVoice2->Start(0);

    // Ожидание
    XAUDIO2_VOICE_STATE state;
    do {
        pSourceVoice1->GetState(&state);
        Sleep(100);
    } while (state.BuffersQueued > 0);
    if (pSourceVoice2) {
        do {
            pSourceVoice2->GetState(&state);
            Sleep(100);
        } while (state.BuffersQueued > 0);
    }

    // Очистка
    if (pSourceVoice2) pSourceVoice2->DestroyVoice();
    pSourceVoice1->DestroyVoice();
    pMasterVoice->DestroyVoice();
    pXAudio2->Release();
    if (pReverb) pReverb->Release();
    CoUninitialize();

    std::cout << "Playback with effects complete.\n";
    return 0;
}