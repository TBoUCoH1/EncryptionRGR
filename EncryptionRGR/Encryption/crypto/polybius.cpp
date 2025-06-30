#include "polybius.h"
#include <stdexcept>
#include <vector>
#include <set>
#include <iostream>

#ifdef _WIN32
#define DLL_EXPORT extern "C" __declspec(dllexport)
#else
#define DLL_EXPORT extern "C"
#endif

// Создание таблицы 16x16 на основе ключа
static vector<vector<unsigned char>> createPolybiusTable(const string& key) {
    vector<vector<unsigned char>> table(16, vector<unsigned char>(16));
    set<unsigned char> used;
    int row = 0, col = 0;

    // Заполнение ключом
    for (char c : key) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (used.find(uc) == used.end()) {
            if (row < 16) { // Проверка на выход за границы
                table[row][col] = uc;
                used.insert(uc);
                col++;
                if (col == 16) {
                    col = 0;
                    row++;
                }
            }
        }
    }

    // Заполнение оставшимися байтами (0–255)
    for (unsigned int i = 0; i < 256; ++i) {
        unsigned char c = static_cast<unsigned char>(i);
        if (used.find(c) == used.end()) {
            if (row < 16) { // Проверка на выход за границы
                table[row][col] = c;
                used.insert(c);
                col++;
                if (col == 16) {
                    col = 0;
                    row++;
                }
            }
        }
    }

    // Проверка, что таблица заполнена
    if (row >= 16 && col > 0) {
        cerr << "Ошибка: таблица Полибия переполнена\n";
        throw runtime_error("Не удалось создать таблицу Полибия");
    }

    return table;
}

// Поиск позиции байта в таблице
static pair<int, int> findPosition(const vector<vector<unsigned char>>& table, unsigned char c) {
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            if (table[i][j] == c) {
                return {i, j};
            }
        }
    }
    throw invalid_argument("Байт не найден в таблице Полибия");
}

// Шифрование
DLL_EXPORT string polybiusEncrypt(const string& text, const string& key) {
    if (key.empty()) throw invalid_argument("Ключ пуст");
    vector<vector<unsigned char>> table = createPolybiusTable(key);
    string result;

    for (char c : text) {
        unsigned char uc = static_cast<unsigned char>(c);
        auto [row, col] = findPosition(table, uc);
        result += static_cast<char>(row);
        result += static_cast<char>(col);
    }

    return result;
}

// Дешифрование
DLL_EXPORT string polybiusDecrypt(const string& text, const string& key) {
    if (key.empty()) throw invalid_argument("Ключ пуст");
    if (text.length() % 2 != 0) throw invalid_argument("Некорректная длина шифротекста");
    vector<vector<unsigned char>> table = createPolybiusTable(key);
    string result;

    for (size_t i = 0; i < text.length(); i += 2) {
        int row = static_cast<unsigned char>(text[i]);
        int col = static_cast<unsigned char>(text[i + 1]);
        if (row >= 16 || col >= 16) {
            throw invalid_argument("Некорректные координаты в шифротексте");
        }
        result += table[row][col];
    }

    return result;
}
