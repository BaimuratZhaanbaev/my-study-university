#include <windows.h>
#include <xaudio2.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <conio.h>

// Подключение библиотеки XAudio2
#pragma comment(lib, "xaudio2.lib")

int main() {
    // Шаг 0: Настройка потоков ввода/вывода и глобальных переменных
    HRESULT hr = S_OK;
    IXAudio2* pXAudio2 = nullptr;
    IXAudio2MasteringVoice* pMasterVoice = nullptr;
    IXAudio2SourceVoice* pSourceVoice = nullptr;
    // Формат аудио считывается из WAV-данных и подстраивается под PCM
    WAVEFORMATEX wfx = {};
    std::vector<uint8_t> wavData;
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
    } header = {};

    // Шаг 1: Ввод пути к WAV-файлу во время выполнения
    std::string filename;
    std::cout << "Enter the full path to input WAV file: ";
    std::getline(std::cin, filename);
    if (filename.empty()) {
        std::cerr << "There is no such file. Exiting.\n";
        return -1;
    }

    // Шаг 2: Инициализация COM-подсистемы и XAudio2
    hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        std::cerr << "Can't initialize COM: " << std::hex << hr << "\n";
        return -1;
    }

    hr = ::XAudio2Create(&pXAudio2, 0);
    if (FAILED(hr)) {
        std::cerr << "Can't create XAudio2: " << std::hex << hr << "\n";
        CoUninitialize();
        return -1;
    }

    // Шаг 3: Создание мастер-голоса (Mastering Voice)
    hr = pXAudio2->CreateMasteringVoice(&pMasterVoice);
    if (FAILED(hr)) {
        std::cerr << "Can't create Mastering Voice: " << std::hex << hr << "\n";
        pXAudio2->Release();
        CoUninitialize();
        return -1;
    }

    // Шаг 4: Встроенная загрузка WAV-файла внутри main
    // Читаем простой RIFF/WAV заголовок и данные без обработки
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Can't open WAV file: " << filename << "\n";
            pMasterVoice->DestroyVoice();
            pXAudio2->Release();
            CoUninitialize();
            return -1;
        }

        // 1) RIFF заголовок
        file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
        if (!file) {
            std::cerr << "Can't read WAV-header.\n";
            file.close();
            pMasterVoice->DestroyVoice();
            pXAudio2->Release();
            CoUninitialize();
            return -1;
        }

        // 2) Чтение "данных" чанка
        wavData.resize(header.dataSize);
        file.read(reinterpret_cast<char*>(wavData.data()), header.dataSize);
        if (!file) {
            std::cerr << "Can't read WAV-data.\n";
            file.close();
            pMasterVoice->DestroyVoice();
            pXAudio2->Release();
            CoUninitialize();
            return -1;
        }
        file.close();

        // Заполняем параметры WAV-файла в WAVEFORMATEX
        wfx.wFormatTag = 1; // WAVE_FORMAT_PCM
        wfx.nChannels = static_cast<WORD>(header.channels);
        wfx.nSamplesPerSec = header.sampleRate;
        wfx.wBitsPerSample = static_cast<WORD>(header.bitsPerSample);
        wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
        wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
        wfx.cbSize = 0;
    }

    // Шаг 5: Создание Source Voice с указанием формата
    hr = pXAudio2->CreateSourceVoice(&pSourceVoice, &wfx);
    if (FAILED(hr)) {
        std::cerr << "Can't create Source Voice: " << std::hex << hr << "\n";
        pMasterVoice->DestroyVoice();
        pXAudio2->Release();
        CoUninitialize();
        return -1;
    }

    // Шаг 6: Подготовка XAUDIO2_BUFFER и передача данных
    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = static_cast<UINT32>(wavData.size());
    buffer.pAudioData = wavData.data();
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    hr = pSourceVoice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr)) {
        std::cerr << "Can't send buffer in Source Voice: " << std::hex << hr << "\n";
        pSourceVoice->DestroyVoice();
        pMasterVoice->DestroyVoice();
        pXAudio2->Release();
        CoUninitialize();
        return -1;
    }

    // Шаг 7: Запуск воспроизведения
    hr = pSourceVoice->Start(0);
    if (FAILED(hr)) {
        std::cerr << "Can't start Source Voice: " << std::hex << hr << "\n";
        pSourceVoice->DestroyVoice();
        pMasterVoice->DestroyVoice();
        pXAudio2->Release();
        CoUninitialize();
        return -1;
    }

    // Шаг 8: Ожидание завершения воспроизведения
    XAUDIO2_VOICE_STATE state;
    do {
        pSourceVoice->GetState(&state);
        Sleep(100);
    } while (state.BuffersQueued > 0);

    // Шаг 9: Очистка ресурсов
    pSourceVoice->DestroyVoice();
    pMasterVoice->DestroyVoice();
    pXAudio2->Release();
    CoUninitialize();

    // Готово
    std::cout << "Playback complete.\n";
    return 0;
}