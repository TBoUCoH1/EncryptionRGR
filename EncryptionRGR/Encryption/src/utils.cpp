#include "utils.h"

// Безопасный ввод числа
bool safeInputInt(int& var, const string& errorMsg) {
    if (!(cin >> var)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cerr << "Ошибка: " << errorMsg << "\n";
        return false;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return true;
}

// Ожидание нажатия Enter
void pauseBeforeExit() {
    cin.clear();
    cout << "\nНажмите Enter для выхода...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Генерация случайного числового ключа
string generateRandomNumericKey() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 9);
    string key;
    for (int i = 0; i < 6; ++i) {
        key += to_string(dis(gen));
    }
    return key;
}

// Проверка валидности числового ключа
bool isNumericKeyValid(const string& key) {
    for (char c : key) {
        if (!isdigit(c)) return false;
    }
    return !key.empty();
}
