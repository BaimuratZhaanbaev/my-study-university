#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int testBMPFile()
{
    string inFileName;
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;
    HANDLE hInputFile = INVALID_HANDLE_VALUE;
    DWORD RW = 0;

    cout << "Enter the full path of input BMP file for testing: ";
    cin >> inFileName;
    cout << "You entered: " << inFileName << endl;

    ifstream testFile(inFileName);
    if (!testFile)
    {
        cout << "Файл не найден или недоступен: " << inFileName << endl;
        return 1;
    }
    testFile.close();

    hInputFile = CreateFileA(
        inFileName.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (hInputFile == INVALID_HANDLE_VALUE)
    {
        cout << "Не могу открыть входной файл! Ошибка: " << GetLastError() << endl;
        return 1;
    }

    ReadFile(
        hInputFile,
        &bmpFileHeader,
        sizeof(bmpFileHeader),
        &RW,
        NULL
    );
    ReadFile(
        hInputFile,
        &bmpInfoHeader,
        sizeof(bmpInfoHeader),
        &RW,
        NULL
    );

    int bitCount = bmpInfoHeader.biBitCount;
    if (bitCount == 8 || bitCount == 24)
    {
        cout << "Width: " << bmpInfoHeader.biWidth << " pixels" << endl;
        cout << "Height: " << bmpInfoHeader.biHeight << " pixels" << endl;
        cout << "Bits per pixel: " << bitCount << endl;
    }
    else
    {
        cout << "Битность не 8 или 24! Программа работает только с этими форматами." << endl;
    }

    CloseHandle(hInputFile);
    return 0;
}

int convertBMPFile()
{
    string inFileName, outFileName;
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;
    int width = 0, height = 0;

    RGBTRIPLE* inBuf = nullptr;
    BYTE* outBuf = nullptr;

    RGBQUAD Palette[2];

    HANDLE hInputFile = INVALID_HANDLE_VALUE;
    HANDLE hOutputFile = INVALID_HANDLE_VALUE;
    DWORD RW = 0;

    cout << "Enter the full path of input BMP file for conversion: ";
    cin >> inFileName;
    cout << "You entered: " << inFileName << endl;

    ifstream testFile(inFileName);
    if (!testFile)
    {
        cout << "Файл не найден или недоступен: " << inFileName << endl;
        return 1;
    }
    testFile.close();

    hInputFile = CreateFileA(
        inFileName.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (hInputFile == INVALID_HANDLE_VALUE)
    {
        cout << "Не могу открыть входной файл! Ошибка: " << GetLastError() << endl;
        return 1;
    }

    cout << "Enter the full path to output BMP file: ";
    cin >> outFileName;

    hOutputFile = CreateFileA(
        outFileName.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (hOutputFile == INVALID_HANDLE_VALUE)
    {
        CloseHandle(hInputFile);
        cout << "Не могу создать выходной файл! Ошибка: " << GetLastError() << endl;
        return 1;
    }

    ReadFile(
        hInputFile,
        &bmpFileHeader,
        sizeof(bmpFileHeader),
        &RW,
        NULL
    );
    ReadFile(
        hInputFile,
        &bmpInfoHeader,
        sizeof(bmpInfoHeader),
        &RW,
        NULL
    );

    SetFilePointer(
        hInputFile,
        bmpFileHeader.bfOffBits,
        NULL,
        FILE_BEGIN
    );

    width = bmpInfoHeader.biWidth;
    height = bmpInfoHeader.biHeight;

    if (bmpInfoHeader.biBitCount != 24)
    {
        cout << "Файл должен быть 24-битным!" << endl;
        CloseHandle(hInputFile);
        CloseHandle(hOutputFile);
        return 1;
    }

    inBuf = new RGBTRIPLE[width];
    int rowSize = (width + 7) / 8;
    outBuf = new BYTE[rowSize];

    bmpFileHeader.bfOffBits = sizeof(bmpFileHeader) + sizeof(bmpInfoHeader) + 2 * sizeof(RGBQUAD);
    bmpInfoHeader.biBitCount = 1;
    int padding = (4 - (rowSize % 4)) % 4;
    bmpFileHeader.bfSize = bmpFileHeader.bfOffBits + (rowSize + padding) * height;

    WriteFile(
        hOutputFile,
        &bmpFileHeader,
        sizeof(bmpFileHeader),
        &RW,
        NULL
    );
    WriteFile(
        hOutputFile,
        &bmpInfoHeader,
        sizeof(bmpInfoHeader),
        &RW,
        NULL
    );

    Palette[0].rgbBlue = 0;
    Palette[0].rgbGreen = 0;
    Palette[0].rgbRed = 0;
    Palette[0].rgbReserved = 0;
    Palette[1].rgbBlue = 255;
    Palette[1].rgbGreen = 255;
    Palette[1].rgbRed = 255;
    Palette[1].rgbReserved = 0;

    WriteFile(
        hOutputFile,
        Palette,
        sizeof(Palette),
        &RW,
        NULL
    );

    int threshold;
    cout << "Enter threshold (0-255): ";
    cin >> threshold;

    for (int y = 0; y < height; y++)
    {
        ReadFile(
            hInputFile,
            inBuf,
            sizeof(RGBTRIPLE) * width,
            &RW,
            NULL
        );

        for (int i = 0; i < rowSize; i++)
        {
            outBuf[i] = 0;
        }

        for (int x = 0; x < width; x++)
        {
            BYTE gray = (BYTE)(0.3 * inBuf[x].rgbtRed + 0.59 * inBuf[x].rgbtGreen + 0.11 * inBuf[x].rgbtBlue);
            if (gray >= threshold) {
                int byteIndex = x / 8;
                int bitPos = 7 - (x % 8);
                outBuf[byteIndex] |= (1 << bitPos);
            }
        }

        WriteFile(
            hOutputFile,
            outBuf,
            rowSize,
            &RW,
            NULL
        );

        if (padding > 0)
        {
            BYTE pad[3] = { 0, 0, 0 };
            WriteFile(
                hOutputFile,
                pad,
                padding,
                &RW,
                NULL
            );
        }

        int inPadding = (4 - ((width * 3) % 4)) % 4;
        if (inPadding) SetFilePointer(
            hInputFile,
            inPadding,
            NULL,
            FILE_CURRENT
        );
    }

    delete[] inBuf;
    delete[] outBuf;
    CloseHandle(hInputFile);
    CloseHandle(hOutputFile);

    cout << "Conversion completed successfully!" << endl;
    return 0;
}

int convertTo8Bit()
{
    string inFileName, outFileName;
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;
    int width = 0, height = 0;

    RGBTRIPLE* inBuf = nullptr;
    BYTE* outBuf = nullptr;

    RGBQUAD Palette[256]; // Палитра для 256 оттенков серого

    HANDLE hInputFile = INVALID_HANDLE_VALUE;
    HANDLE hOutputFile = INVALID_HANDLE_VALUE;
    DWORD RW = 0;

    cout << "Введите полный путь к входному файлу BMP для 8-битного преобразования: ";
    cin >> inFileName;
    cout << "Вы ввели: " << inFileName << endl;

    ifstream testFile(inFileName);
    if (!testFile)
    {
        cout << "Файл не найден или недоступен: " << inFileName << endl;
        return 1;
    }
    testFile.close();

    hInputFile = CreateFileA(
        inFileName.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (hInputFile == INVALID_HANDLE_VALUE)
    {
        cout << "Не могу открыть входной файл! Ошибка: " << GetLastError() << endl;
        return 1;
    }

    cout << "Введите полный путь к выходному файлу BMP.: ";
    cin >> outFileName;

    hOutputFile = CreateFileA(
        outFileName.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (hOutputFile == INVALID_HANDLE_VALUE)
    {
        CloseHandle(hInputFile);
        cout << "Не могу создать выходной файл! Ошибка: " << GetLastError() << endl;
        return 1;
    }

    ReadFile(
        hInputFile,
        &bmpFileHeader,
        sizeof(bmpFileHeader),
        &RW,
        NULL
    );
    ReadFile(
        hInputFile,
        &bmpInfoHeader,
        sizeof(bmpInfoHeader),
        &RW,
        NULL
    );

    SetFilePointer(
        hInputFile,
        bmpFileHeader.bfOffBits,
        NULL,
        FILE_BEGIN
    );

    width = bmpInfoHeader.biWidth;
    height = bmpInfoHeader.biHeight;

    if (bmpInfoHeader.biBitCount != 24)
    {
        cout << "Файл должен быть 24-битным!" << endl;
        CloseHandle(hInputFile);
        CloseHandle(hOutputFile);
        return 1;
    }

    inBuf = new RGBTRIPLE[width];
    outBuf = new BYTE[width];

    bmpFileHeader.bfOffBits = sizeof(bmpFileHeader) + sizeof(bmpInfoHeader) + 256 * sizeof(RGBQUAD);
    bmpInfoHeader.biBitCount = 8;
    int rowSize = width;
    int padding = (4 - (rowSize % 4)) % 4;
    bmpFileHeader.bfSize = bmpFileHeader.bfOffBits + (rowSize + padding) * height;

    WriteFile(
        hOutputFile,
        &bmpFileHeader,
        sizeof(bmpFileHeader),
        &RW,
        NULL
    );
    WriteFile(
        hOutputFile,
        &bmpInfoHeader,
        sizeof(bmpInfoHeader),
        &RW,
        NULL
    );

    for (int i = 0; i < 256; i++)
    {
        Palette[i].rgbBlue = (BYTE)i;
        Palette[i].rgbGreen = (BYTE)i;
        Palette[i].rgbRed = (BYTE)i;
        Palette[i].rgbReserved = 0;
    }
    WriteFile(hOutputFile, Palette, 256 * sizeof(RGBQUAD), &RW, NULL);

    for (int y = 0; y < height; y++)
    {
        ReadFile(
            hInputFile,
            inBuf,
            sizeof(RGBTRIPLE) * width,
            &RW,
            NULL
        );

        for (int x = 0; x < width; x++)
        {
            BYTE gray = (BYTE)(0.3 * inBuf[x].rgbtRed + 0.59 * inBuf[x].rgbtGreen + 0.11 * inBuf[x].rgbtBlue);
            outBuf[x] = gray;
        }

        WriteFile(
            hOutputFile,
            outBuf,
            rowSize,
            &RW,
            NULL
        );

        if (padding > 0)
        {
            BYTE pad[3] = { 0, 0, 0 };
            WriteFile(
                hOutputFile,
                pad,
                padding,
                &RW,
                NULL
            );
        }

        int inPadding = (4 - ((width * 3) % 4)) % 4;
        if (inPadding) SetFilePointer(
            hInputFile,
            inPadding,
            NULL,
            FILE_CURRENT
        );
    }

    delete[] inBuf;
    delete[] outBuf;
    CloseHandle(hInputFile);
    CloseHandle(hOutputFile);

    cout << "Conversion to 8-bit grayscale completed successfully!" << endl;
    return 0;
}

int main()
{
    setlocale(LC_ALL, "RUS");

    int choice;
    cout << "Лабораторная работа конвертер BMP" << endl;
    cout << "1. Проверить файл BMP" << endl;
    cout << "2. Конвертировать 24-битный BMP в 1-битный" << endl;
    cout << "3. Конвертировать 24-битный BMP в 8-битный (исходная программа)" << endl;
    cout << "Введите свой выбор (1, 2 или 3): ";
    cin >> choice;

    if (choice == 1)
    {
        return testBMPFile();
    }
    else if (choice == 2)
    {
        return convertBMPFile();
    }
    else if (choice == 3)
    {
        return convertTo8Bit();
    }
    else
    {
        cout << "Неверный выбор! Введите 1, 2 или 3." << endl;
        system("пауза");
        return 1;
    }
}
