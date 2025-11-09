// ... (предыдущие include и struct WAVHeader)

// Функция для применения простого эхо (offline)
void ApplyEcho(std::vector<uint8_t>& data, int channels, int bitsPerSample, int sampleRate) {
    if (bitsPerSample != 16) return; // Только для 16-bit
    int bytesPerSample = bitsPerSample / 8;
    int delaySamples = static_cast<int>(0.3 * sampleRate); // Задержка 0.3 сек
    float attenuation = 0.5f;

    std::vector<int16_t> samples(data.size() / bytesPerSample);
    memcpy(samples.data(), data.data(), data.size());

    std::vector<int16_t> echoed(samples.size(), 0);
    for (size_t i = 0; i < samples.size(); ++i) {
        echoed[i] = samples[i];
        if (i >= delaySamples * channels) {
            echoed[i] += static_cast<int16_t>(samples[i - delaySamples * channels] * attenuation);
            if (echoed[i] > INT16_MAX) echoed[i] = INT16_MAX;
            if (echoed[i] < INT16_MIN) echoed[i] = INT16_MIN;
        }
    }

    data.resize(echoed.size() * bytesPerSample);
    memcpy(data.data(), echoed.data(), data.size());
}

// Функция для сохранения WAV с добавлением INFO chunk
HRESULT SaveWAVWithInfo(const std::string& outputFilename, const WAVEFORMATEX& wfx, const std::vector<uint8_t>& wavData, const std::string& artistName) {
    std::ofstream file(outputFilename, std::ios::binary);
    if (!file) return E_FAIL;

    // Основной header
    WAVHeader header = {};
    header.riff = 0x46464952; // 'RIFF'
    header.wave = 0x45564157; // 'WAVE'
    header.fmt = 0x20746D66; // 'fmt '
    header.fmtSize = 16;
    header.audioFormat = 1;
    header.channels = wfx.nChannels;
    header.sampleRate = wfx.nSamplesPerSec;
    header.bitsPerSample = wfx.wBitsPerSample;
    header.blockAlign = (header.channels * header.bitsPerSample) / 8;
    header.avgBytesPerSec = header.sampleRate * header.blockAlign;
    header.data = 0x61746164; // 'data'
    header.dataSize = static_cast<uint32_t>(wavData.size());

    // Размер LIST INFO
    std::string infoData = "LIST";
    uint32_t listSize = 4 + 8 + artistName.size() + (artistName.size() % 2 ? 1 : 0); // INFO + IART + data + padding
    infoData.append(reinterpret_cast<const char*>(&listSize), 4);
    infoData += "INFO";
    infoData += "IART";
    uint32_t nameSize = static_cast<uint32_t>(artistName.size());
    infoData.append(reinterpret_cast<const char*>(&nameSize), 4);
    infoData += artistName;
    if (artistName.size() % 2) infoData += '\0'; // Padding

    // Полный размер файла
    header.size0 = 4 + sizeof(WAVHeader) - 8 + header.dataSize + 8 + listSize; // RIFF size

    file.write(reinterpret_cast<const char*>(&header), sizeof(WAVHeader));
    file.write(reinterpret_cast<const char*>(wavData.data()), wavData.size());
    file.write(infoData.data(), infoData.size());

    return S_OK;
}

int main() {
    // ... (инициализация как в задании 3, загрузка filename1)

    // Применение эхо к данным
    ApplyEcho(wavData1, wfx1.nChannels, wfx1.wBitsPerSample, wfx1.nSamplesPerSec);

    // Сохранение с именем в header
    std::string artist = "Grok"; // Ваше имя
    hr = SaveWAVWithInfo("output_with_effects.wav", wfx1, wavData1, artist);
    if (FAILED(hr)) { std::cerr << "Can't save modified WAV.\n"; }

    // Проигрывание модифицированного (reload для теста)
    std::cout << "Saved modified WAV with echo and artist name. Now playing...\n";
    // ... (создание source voice и проигрывание как в задании 3, но с "output_with_effects.wav")

    // Очистка...
}