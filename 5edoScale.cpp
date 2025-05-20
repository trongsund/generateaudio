// 5edoScale.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <conio.h>
#include <math.h>
#include <vector>
#include <array>

struct WAVHEADER
{
    // WAV-формат начинается с RIFF-заголовка:

    // Содержит символы "RIFF" в ASCII кодировке
    // (0x52494646 в big-endian представлении)
    unsigned int chunkId;

    // 36 + subchunk2Size, или более точно:
    // 4 + (8 + subchunk1Size) + (8 + subchunk2Size)
    // Это оставшийся размер цепочки, начиная с этой позиции.
    // Иначе говоря, это размер файла - 8, то есть,
    // исключены поля chunkId и chunkSize.
    unsigned long chunkSize;

    // Содержит символы "WAVE"
    // (0x57415645 в big-endian представлении)
    unsigned int format;

    // Формат "WAVE" состоит из двух подцепочек: "fmt " и "data":
    // Подцепочка "fmt " описывает формат звуковых данных:

    // Содержит символы "fmt "
    // (0x666d7420 в big-endian представлении)
    unsigned int subchunk1Id;

    // 16 для формата PCM.
    // Это оставшийся размер подцепочки, начиная с этой позиции.
    unsigned long subchunk1Size;

    // Аудио формат, полный список можно получить здесь http://audiocoding.ru/wav_formats.txt
    // Для PCM = 1 (то есть, Линейное квантование).
    // Значения, отличающиеся от 1, обозначают некоторый формат сжатия.
    unsigned short audioFormat;

    // Количество каналов. Моно = 1, Стерео = 2 и т.д.
    unsigned short numChannels;

    // Частота дискретизации. 8000 Гц, 44100 Гц и т.д.
    unsigned long sampleRate;

    // sampleRate * numChannels * bitsPerSample/8
    unsigned long byteRate;

    // numChannels * bitsPerSample/8
    // Количество байт для одного сэмпла, включая все каналы.
    unsigned short blockAlign;

    // Так называемая "глубиная" или точность звучания. 8 бит, 16 бит и т.д.
    unsigned short bitsPerSample;

    // Подцепочка "data" содержит аудио-данные и их размер.

    // Содержит символы "data"
    // (0x64617461 в big-endian представлении)
    unsigned int subchunk2Id;

    // numSamples * numChannels * bitsPerSample/8
    // Количество байт в области данных.
    unsigned long subchunk2Size;

    // Далее следуют непосредственно Wav данные.
};

int main()
{
    WAVHEADER header;
    int length_f = 24, samplerate = 48000;
    header.chunkId = 0x46464952;
    header.chunkSize = 36 + length_f * samplerate * 3;
    header.format = 0x45564157;
    header.subchunk1Id = 0x20746d66;
    header.subchunk1Size = 16;
    header.audioFormat = 1;
    header.numChannels = 1;
    header.sampleRate = samplerate;
    header.byteRate = samplerate * 3;
    header.blockAlign = 3;
    header.bitsPerSample = 24;
    header.subchunk2Id = 0x61746164;
    header.subchunk2Size = length_f * samplerate * 3;

    std::ofstream f_out("5edoScale_soft.wav", std::ios::binary);
    f_out.write(reinterpret_cast<char*>(&header), 44);

    int value = 0, zero = 0;

    double cis = 220. * pow(2, 1. / 3.), freq = 0.;

    std::array<int, 11> pows = { 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0 };

    for (int i = 0; i < 11; ++i) {
        freq = cis * pow(2, 0.2 * pows[i]);
        for (int j = 0; j < samplerate * 2; ++j) {
            if (freq * j / samplerate < 8) {
                value = int(trunc(4194304 * sin(freq * j / samplerate * 2 * M_PI)
                        * (1. - fabs(j / (samplerate / freq * 4) - 1))));
                f_out.write(reinterpret_cast<char*>(&value), 3);
            }
            else {
                f_out.write(reinterpret_cast<char*>(&zero), 3);
            }
        }
    }

    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
