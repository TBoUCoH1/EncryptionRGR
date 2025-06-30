#include "caesar.h"
#include <stdexcept>
#include <cctype>

#ifdef _WIN32
#define DLL_EXPORT extern "C" __declspec(dllexport)
#else
#define DLL_EXPORT extern "C"
#endif

// Шифр Цезаря
DLL_EXPORT string caesarEncrypt(const string& text, const string& key) {
    if (key.empty()) throw invalid_argument("Ключ пуст");
    for (char c : key) {
        if (!isdigit(c)) throw invalid_argument("Ключ должен содержать только цифры");
    }
    int shift = stoi(key) % 256;
    string result;
    for (unsigned char c : text) {
        result += static_cast<unsigned char>((c + shift) % 256);
    }
    return result;
}

DLL_EXPORT string caesarDecrypt(const string& text, const string& key) {
    if (key.empty()) throw invalid_argument("Ключ пуст");
    for (char c : key) {
        if (!isdigit(c)) throw invalid_argument("Ключ должен содержать только цифры");
    }
    int shift = stoi(key) % 256;
    string result;
    for (unsigned char c : text) {
        result += static_cast<unsigned char>((c - shift + 256) % 256);
    }
    return result;
}