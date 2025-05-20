// GenerateWAV.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
#include <conio.h>
#include <math.h>
#include <vector>
#include <functional>
#include <string>

struct Polynomial {
    std::vector<long double> coeffs;

    Polynomial() : coeffs(std::vector<long double>{0}) {}

    Polynomial(long double free_term) : coeffs(std::vector<long double>{free_term}) {}

    void diff_gauss() {
//        std::vector<int64_t> new_coeffs = coeffs;
//        new_coeffs.push_back(2 * coeffs[coeffs.size() - 1]);
//        for (size_t i = coeffs.size() - 1; i > 0; --i) {
//            new_coeffs[i] = 2 * new_coeffs[i - 1];
//        }
//        new_coeffs[0] = 0;
//        for (size_t i = 1; i < coeffs.size(); ++i) {
//            new_coeffs[i - 1] -= coeffs[i] * i;
//        }
//        coeffs = new_coeffs;
        size_t deg = coeffs.size();
        coeffs.push_back(0);
        for (size_t i = deg; i > 0; --i) {
            coeffs[i] = coeffs[i - 1];
        }
        coeffs[0] = 0;
        for (size_t i = 0; i < deg - 1; ++i) {
            coeffs[i] -= coeffs[i + 2] * (i + 1);
        }
    }

    template<typename T>
    T operator() (T x) {
        T result = 0;
        for (size_t i = coeffs.size(); i > 0; --i) {
            result *= x;
            result += coeffs[i - 1];
        }
        return result;
    }
};

double factorial(int n) {
    double result = 1;
    for (int i = 1; i <= n; ++i) {
        result *= i;
    }
    return result;
}

std::vector <int> gaussian_table(Polynomial rule, double left, double right, size_t size, int mult) {
    std::vector<int> result(size);
    double arg;
    int deg = rule.coeffs.size();
    for (size_t i = 0; i < size; ++i) {
        arg = left + (right - left) / size * i;
        result[i] = int(rule(arg) * exp (-arg * arg / 2) / pow(factorial(deg), 0.5) * sqrt(deg) * mult);
    }
    return result;
}

//std::function<double(double)> gaussian(Polynomial pol) {
//    return [&pol](double x) {return pol(x) * exp(-x * x / 2); };
//}

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
    unsigned int length_f, samplerate, number_harm;
    std::cout << "Length: ";
    std::cin >> length_f;
    std::cout << "Sample rate: ";
    std::cin >> samplerate;
    std::cout << "Number of harmonics: ";
    std::cin >> number_harm;
    WAVHEADER header;
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

    int plus = 4194304, minus = -4194304, zero = 0;
    int index = 0;

    std::ofstream f_out("Gauss_" + std::to_string(number_harm) + ".wav", std::ios::binary);
    f_out.write(reinterpret_cast<char*>(&header), 44);

    size_t samples = length_f * samplerate;

    //auto is_compose = eratosthenes_sieve(samples + 8);

    int odd = 0, step = 0;
    size_t ind = 0, gap = 0;

    //int factor = 1;

    std::vector<int> waveform;
    Polynomial level(1.);
    //std::function<double(double)> rule;

    for (unsigned int harm = 0; harm < number_harm; ++harm) {
        //rule = gaussian(level);
        //std::cout << rule(0) << ' ';
        //for (int i = 0; i <= harm; ++i) {
        //    std::cout << level.coeffs[i] << ' ';
        //}
        std::cout << std::endl;
        waveform = gaussian_table(level, -500, 500, samplerate, plus);
        for (size_t i = 0; i < samplerate; ++i) {
            //waveform[i] /= factor;
            f_out.write(reinterpret_cast<char*>(&(waveform[i])), 3);
            if (waveform[i] != 0) {
                std::cout << waveform[i] << ' ';
            }
        }
        level.diff_gauss();
        std::cout << std::endl;
        //factor *= harm + 1;
    }
}

//    for (size_t i = 0; i < samples; ++i) {
        //index = short(trunc(sin(log1p(log1p(i)) * 10000) * 24576));
        //f_out.write(reinterpret_cast<char*>(&index), 2);
 //       if (/*is_square(i) */!is_compose[i]/*(i & (i - 1)) == 0*/) {
//            ind = i + 1;
//            while (ind < samples && is_compose[ind]) ++ind;
//            gap = ind - i;
//            step = 8388608 / gap;
//            if (odd % 2 == 1) {
//                f_out.write(reinterpret_cast<char*>(&minus), 3);
//                index = minus;
//            }
//           else {
//                f_out.write(reinterpret_cast<char*>(&plus), 3);
//                index = plus;
//                step *= -1;
//            }
//            ++odd;
//            //std::cout << i << ' ';
//        }
//        else {
//            index += step;
//            f_out.write(reinterpret_cast<char*>(&index), 3);
//        }
//    }
//    return 0;
//}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
