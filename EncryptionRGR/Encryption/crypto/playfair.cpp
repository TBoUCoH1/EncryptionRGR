#include "playfair.h"
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
static vector<vector<unsigned char>> createPlayfairTable(const string& key) {
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
        cerr << "Ошибка: таблица Плейфера переполнена\n";
        throw runtime_error("Не удалось создать таблицу Плейфера");
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
    throw invalid_argument("Байт не найден в таблице Плейфера");
}

// Шифрование
DLL_EXPORT string playfairEncrypt(const string& text, const string& key) {
    if (key.empty()) throw invalid_argument("Ключ пуст");
    vector<vector<unsigned char>> table = createPlayfairTable(key);
    string result;
    size_t len = text.length();
    string paddedText = text;

    // Добавить заполнитель (0x00), если длина нечётная
    if (len % 2 != 0) {
        paddedText += '\0';
        len++;
    }

    for (size_t i = 0; i < len; i += 2) {
        unsigned char c1 = static_cast<unsigned char>(paddedText[i]);
        unsigned char c2 = static_cast<unsigned char>(paddedText[i + 1]);
        auto [r1, c1_pos] = findPosition(table, c1);
        auto [r2, c2_pos] = findPosition(table, c2);

        if (r1 == r2) {
            // В одной строке: сдвиг вправо
            result += table[r1][(c1_pos + 1) % 16];
            result += table[r2][(c2_pos + 1) % 16];
        } else if (c1_pos == c2_pos) {
            // В одном столбце: сдвиг вниз
            result += table[(r1 + 1) % 16][c1_pos];
            result += table[(r2 + 1) % 16][c2_pos];
        } else {
            // Прямоугольник: обмен столбцами
            result += table[r1][c2_pos];
            result += table[r2][c1_pos];
        }
    }

    return result;
}

// Дешифрование
DLL_EXPORT string playfairDecrypt(const string& text, const string& key) {
    if (key.empty()) throw invalid_argument("Ключ пуст");
    if (text.length() % 2 != 0) throw invalid_argument("Некорректная длина шифротекста");
    vector<vector<unsigned char>> table = createPlayfairTable(key);
    string result;

    for (size_t i = 0; i < text.length(); i += 2) {
        unsigned char c1 = static_cast<unsigned char>(text[i]);
        unsigned char c2 = static_cast<unsigned char>(text[i + 1]);
        auto [r1, c1_pos] = findPosition(table, c1);
        auto [r2, c2_pos] = findPosition(table, c2);

        if (r1 == r2) {
            // В одной строке: сдвиг влево
            result += table[r1][(c1_pos - 1 + 16) % 16];
            result += table[r2][(c2_pos - 1 + 16) % 16];
        } else if (c1_pos == c2_pos) {
            // В одном столбце: сдвиг вверх
            result += table[(r1 - 1 + 16) % 16][c1_pos];
            result += table[(r2 - 1 + 16) % 16][c2_pos];
        } else {
            // Прямоугольник: обмен столбцами
            result += table[r1][c2_pos];
            result += table[r2][c1_pos];
        }
    }

    // Удалить заполнитель (0x00), если он был добавлен
    while (!result.empty() && result.back() == '\0') {
        result.pop_back();
    }

    return result;
}
