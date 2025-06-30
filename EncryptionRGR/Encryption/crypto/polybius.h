#pragma once
#include <string>

using namespace std;

#ifdef _WIN32
#define DLL_EXPORT extern "C" __declspec(dllexport)
#else
#define DLL_EXPORT extern "C"
#endif

// Шифр Полибия
DLL_EXPORT string polybiusEncrypt(const string& text, const string& key);
DLL_EXPORT string polybiusDecrypt(const string& text, const string& key);