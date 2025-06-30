#include <sstream>
#include <cstdint>
#include <locale>
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#include "dynamic_loader.h"
#else
#include <dlfcn.h>
#endif
#include "utils.h"

// Типы функций для шифрования/дешифрования
using EncryptFunc = string(*)(const string&, const string&);
using DecryptFunc = string(*)(const string&, const string&);

// Указатели на функции шифров
EncryptFunc caesarEncryptFunc = nullptr;
DecryptFunc caesarDecryptFunc = nullptr;
EncryptFunc playfairEncryptFunc = nullptr;
DecryptFunc playfairDecryptFunc = nullptr;
EncryptFunc polybiusEncryptFunc = nullptr;
DecryptFunc polybiusDecryptFunc = nullptr;

// Загрузка библиотек шифрования
#ifdef _WIN32
const char* caesarPath = "crypto/libcaesar.dll";
const char* playfairPath = "crypto/libplayfair.dll";
const char* polybiusPath = "crypto/libpolybius.dll";
#else
const char* caesarPath = "./build/crypto/libcaesar.so";
const char* playfairPath = "./build/crypto/libplayfair.so";
const char* polybiusPath = "./build/crypto/libpolybius.so";
#endif

void* caesarLib = nullptr;
void* playfairLib = nullptr;
void* polybiusLib = nullptr;

bool caesarAvailable = false;
bool playfairAvailable = false;
bool polybiusAvailable = false;

void loadLibraries() {
#ifdef _WIN32
    caesarLib = loadLibrary(caesarPath);
    playfairLib = loadLibrary(playfairPath);
    polybiusLib = loadLibrary(polybiusPath);
#else
    caesarLib = dlopen(caesarPath, RTLD_LAZY);
    playfairLib = dlopen(playfairPath, RTLD_LAZY);
    polybiusLib = dlopen(polybiusPath, RTLD_LAZY);
#endif
    caesarAvailable = (caesarLib != nullptr);
    playfairAvailable = (playfairLib != nullptr);
    polybiusAvailable = (polybiusLib != nullptr);
}

void closeLibraries() {
#ifdef _WIN32
    if (caesarLib) closeLibrary(caesarLib);
    if (playfairLib) closeLibrary(playfairLib);
    if (polybiusLib) closeLibrary(polybiusLib);
#else
    if (caesarLib) dlclose(caesarLib);
    if (playfairLib) dlclose(playfairLib);
    if (polybiusLib) dlclose(polybiusLib);
#endif
}

// Функция для проверки ввода целого числа
bool getValidInt(int& value, const string& prompt, int minVal, int maxVal) {
    while (true) {
        cout << prompt;
        string line;
        getline(cin, line);

        try {
            size_t pos;
            value = stoi(line, &pos);
            if (pos != line.length() || value < minVal || value > maxVal) {
                cerr << "Ошибка: введите число от " << minVal << " до " << maxVal << ".\n";
            } else {
                return true;
            }
        } catch (...) {
            cerr << "Ошибка: введите корректное число.\n";
        }
    }
}



// Проверка пароля
bool authenticateUser() {
    const string correctPassword = "1111";
    const int maxAttempts = 4;
    string userInput;

    for (int attempt = 1; attempt <= maxAttempts; ++attempt) {
        cout << "\nПопытка " << attempt << " из " << maxAttempts << ". Введите пароль: ";
        getline(cin, userInput);
        if (userInput == correctPassword) {
            cout << "Пароль верный. Доступ предоставлен.\n";
            return true;
        }
        cout << "Неверный пароль. Осталось попыток: " << (maxAttempts - attempt) << "\n";
    }
    cerr << "Превышено количество попыток. Доступ заблокирован.\n";
    return false;
}

// Получение входных данных
bool getInputData(string& inputText, string& sourceFile, bool& isText, bool& shouldExit) {
    cout << "\nВыберите источник данных:\n1. Файл\n2. Консоль\nВведите 'exit' для выхода.\nВаш выбор: ";
    string line;
    getline(cin, line);

    if (line == "exit") {
        shouldExit = true;
        return true;
    }

    int choice;
    try {
        size_t pos;
        choice = stoi(line, &pos);
        if (pos != line.length() || (choice != 1 && choice != 2)) {
            cerr << "Ошибка: введите 1, 2 или 'exit'.\n";
            return false;
        }
    } catch (...) {
        cerr << "Ошибка: введите 1, 2 или 'exit'.\n";
        return false;
    }

    if (choice == 1) {
        cout << "Введите имя файла: ";
        getline(cin, sourceFile);
        ifstream file(sourceFile, ios::binary);
        if (!file) {
            cerr << "Ошибка: не удалось открыть файл '" << sourceFile << "'.\n";
            return false;
        }
        ostringstream buffer;
        buffer << file.rdbuf();
        inputText = buffer.str();
        file.close();
        cout << "Данные из файла успешно загружены.\n";
        isText = false;
    } else {
        cout << "Введите текст: ";
        getline(cin, inputText);
        isText = true;
    }

    return true;
}


// Выбор шифра
bool selectCipher(CipherType& selectedCipher) {
    cout << "\nДоступные шифры:\n";
    int option = 1;
    if (caesarAvailable) cout << option++ << ". Шифр Цезаря\n";
    if (playfairAvailable) cout << option++ << ". Шифр Плейфера\n";
    if (polybiusAvailable) cout << option++ << ". Шифр Полибия\n";

    if (!caesarAvailable && !playfairAvailable && !polybiusAvailable) {
        cerr << "Ошибка: нет доступных шифров.\n";
        return false;
    }

    int choice;
    if (!getValidInt(choice, "Выберите шифр: ", 1, option - 1)) {
        return false;
    }

    if (caesarAvailable && choice == 1) {
        selectedCipher = CipherType::Caesar;
    } else if (playfairAvailable && ((caesarAvailable && choice == 2) || (!caesarAvailable && choice == 1))) {
        selectedCipher = CipherType::Playfair;
    } else if (polybiusAvailable && ((caesarAvailable && playfairAvailable && choice == 3) ||
                                     (caesarAvailable && !playfairAvailable && choice == 2) ||
                                     (!caesarAvailable && playfairAvailable && choice == 2) ||
                                     (!caesarAvailable && !playfairAvailable && choice == 1))) {
        selectedCipher = CipherType::Polybius;
    } else {
        cerr << "Ошибка: выбран недоступный шифр.\n";
        return false;
    }
    return true;
}

// Выбор действия
bool selectAction(ActionType& selectedAction) {
    string input;
    cout << "Выберите действие:\n1. Зашифровать\n2. Расшифровать\nВаш выбор: ";
    getline(cin, input);

    try {
        int choice = stoi(input);
        if (choice == 1 || choice == 2) {
            selectedAction = static_cast<ActionType>(choice);
            return true;
        } else {
            cerr << "Ошибка: введите 1 или 2.\n";
        }
    } catch (...) {
        cerr << "Ошибка: введите корректное число.\n";
    }

    return false;
}


// Получение ключа
bool getEncryptionKey(string& key, CipherType cipher) {
    int choice;
    if (!getValidInt(choice, "Сгенерировать ключ автоматически?\n1. Нет\n2. Да\nВаш выбор: ", 1, 2)) {
        return false;
    }

    if (choice == 2) {
        key = generateRandomNumericKey();
        cout << "Автоматически сгенерирован ключ: " << key << "\n";
    } else {
        cout << "Введите ключ вручную: ";
        getline(cin, key);
        if (key.empty()) {
            cerr << "Ошибка: ключ не может быть пустым.\n";
            return false;
        }
        if (cipher == CipherType::Caesar && !isNumericKeyValid(key)) {
            cerr << "Ошибка: ключ для шифра Цезаря должен содержать только цифры.\n";
            return false;
        }
    }
    return true;
}

int main() {
    try {
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        locale::global(locale(""));
#endif

        loadLibraries();

        if (!authenticateUser()) {
            pauseBeforeExit();
            closeLibraries();
            return 1;
        }

        // Загрузка функций
        if (caesarAvailable) {
            caesarEncryptFunc =
#ifdef _WIN32
                (EncryptFunc)getFunction(caesarLib, "caesarEncrypt");
#else
                (EncryptFunc)dlsym(caesarLib, "caesarEncrypt");
#endif
            caesarDecryptFunc =
#ifdef _WIN32
                (DecryptFunc)getFunction(caesarLib, "caesarDecrypt");
#else
                (DecryptFunc)dlsym(caesarLib, "caesarDecrypt");
#endif
        }
        if (playfairAvailable) {
            playfairEncryptFunc =
#ifdef _WIN32
                (EncryptFunc)getFunction(playfairLib, "playfairEncrypt");
#else
                (EncryptFunc)dlsym(playfairLib, "playfairEncrypt");
#endif
            playfairDecryptFunc =
#ifdef _WIN32
                (DecryptFunc)getFunction(playfairLib, "playfairDecrypt");
#else
                (DecryptFunc)dlsym(playfairLib, "playfairDecrypt");
#endif
        }
        if (polybiusAvailable) {
            polybiusEncryptFunc =
#ifdef _WIN32
                (EncryptFunc)getFunction(polybiusLib, "polybiusEncrypt");
#else
                (EncryptFunc)dlsym(polybiusLib, "polybiusEncrypt");
#endif
            polybiusDecryptFunc =
#ifdef _WIN32
                (DecryptFunc)getFunction(polybiusLib, "polybiusDecrypt");
#else
                (DecryptFunc)dlsym(polybiusLib, "polybiusDecrypt");
#endif
        }

        // Главный цикл
        while (true) {
            string inputText, sourceFile, key;
            bool isText = false;
            CipherType selectedCipher;
            ActionType selectedAction;
            bool shouldExit = false;

            if (!getInputData(inputText, sourceFile, isText, shouldExit)) continue;
            if (shouldExit) break;
            if (!selectCipher(selectedCipher)) continue;
            if (!selectAction(selectedAction)) continue;
            if (shouldExit) break;
            if (!getEncryptionKey(key, selectedCipher)) continue;

            string output;
            switch (selectedCipher) {
                case CipherType::Caesar:
                    if (!caesarEncryptFunc || !caesarDecryptFunc) {
                        cerr << "Ошибка: функции Цезаря недоступны.\n";
                        continue;
                    }
                    output = (selectedAction == ActionType::Encrypt)
                        ? caesarEncryptFunc(inputText, key)
                        : caesarDecryptFunc(inputText, key);
                    break;
                case CipherType::Playfair:
                    if (!playfairEncryptFunc || !playfairDecryptFunc) {
                        cerr << "Ошибка: функции Плейфера недоступны.\n";
                        continue;
                    }
                    output = (selectedAction == ActionType::Encrypt)
                        ? playfairEncryptFunc(inputText, key)
                        : playfairDecryptFunc(inputText, key);
                    break;
                case CipherType::Polybius:
                    if (!polybiusEncryptFunc || !polybiusDecryptFunc) {
                        cerr << "Ошибка: функции Полибия недоступны.\n";
                        continue;
                    }
                    output = (selectedAction == ActionType::Encrypt)
                        ? polybiusEncryptFunc(inputText, key)
                        : polybiusDecryptFunc(inputText, key);
                    break;
                default:
                    cerr << "Ошибка: неизвестный шифр.\n";
                    continue;
            }

            string outputFilename = "output" + string(isText ? ".txt" : ".bin");
            ofstream outFile(outputFilename, ios::binary);
            if (!outFile) {
                cerr << "Ошибка: не удалось создать файл '" << outputFilename << "'.\n";
                continue;
            }
            outFile << output;
            outFile.close();
            cout << "Результат сохранен в файл: " << outputFilename << "\n\n";
        }

        cout << "Программа завершена.\n";
    } catch (const exception& e) {
        cerr << "Произошла ошибка: " << e.what() << "\n";
    }
    closeLibraries();
    return 0;
}

