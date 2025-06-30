#pragma once
#include <string>
using namespace std;

#ifdef _WIN32
#define DLL_EXPORT extern "C" __declspec(dllexport)
#else
#define DLL_EXPORT extern "C"
#endif

// Шифр Цезаря
DLL_EXPORT string caesarEncrypt(const string& text, const string& key);
DLL_EXPORT string caesarDecrypt(const string& text, const string& key);