#pragma once
#include <string>

using namespace std;

#ifdef _WIN32
#define DLL_EXPORT extern "C" __declspec(dllexport)
#else
#define DLL_EXPORT extern "C"
#endif

// Шифр Плейфера
DLL_EXPORT string playfairEncrypt(const string& text, const string& key);
DLL_EXPORT string playfairDecrypt(const string& text, const string& key);