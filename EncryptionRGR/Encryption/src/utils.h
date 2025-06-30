#pragma once
#include <iostream>
#include <fstream>
#include <thread>
#include <limits>
#include <random>
#include <string>

using namespace std;

// Типы шифров
enum class CipherType {
    Caesar = 1,
    Playfair = 2,
    Polybius = 3
};

// Типы действий
enum class ActionType {
    Encrypt = 1,
    Decrypt = 2
};

// Безопасный ввод
bool safeInputInt(int& var, const string& errorMsg);
void pauseBeforeExit();
string generateRandomNumericKey();
bool isNumericKeyValid(const string& key);
