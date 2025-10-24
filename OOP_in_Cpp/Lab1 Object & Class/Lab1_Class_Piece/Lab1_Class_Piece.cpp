#include <iostream>
#include <string>
#include <io.h>
#include <fcntl.h>

enum class Color { White, Black };  // Перечисляемый тип — один из трёх разных типов

class Piece {
private:
    char type;           // 'P', 'R', 'N', 'B', 'Q', 'K'
    int row;             // строка (0–7)
    int col;             // столбец (0–7)
    Color color;         // цвет: White / Black
    std::string name;    // полное имя фигуры

public:
    // 1. Конструктор по умолчанию — используем this явно
    Piece() {
        this->type = 'P';
        this->row = 0;
        this->col = 0;
        this->color = Color::White;
        this->name = "White Pawn";
        std::cout << "Конструктор по умолчанию: " << name << std::endl;
    }

    // 2. Конструктор с параметрами — СПИСОК ИНИЦИАЛИЗАТОРОВ
    Piece(char t, int r, int c, Color clr)
        : type(t), row(r), col(c), color(clr)  // <-- список инициализаторов
    {
        // Формируем имя
        std::string colorStr = (clr == Color::White) ? "White " : "Black ";
        switch (std::tolower(t)) {
        case 'p': name = colorStr + "Pawn"; break;
        case 'r': name = colorStr + "Rook"; break;
        case 'n': name = colorStr + "Knight"; break;
        case 'b': name = colorStr + "Bishop"; break;
        case 'q': name = colorStr + "Queen"; break;
        case 'k': name = colorStr + "King"; break;
        default:  name = colorStr + "Unknown"; break;
        }
        std::cout << "Конструктор с параметрами: " << name << " at (" << row << "," << col << ")" << std::endl;
    }

    // 3. Конструктор копирования
    Piece(const Piece& other) {
        type = other.type;
        row = other.row;
        col = other.col;
        color = other.color;
        name = other.name + " (копия)";
        std::cout << "Конструктор копирования: " << name << std::endl;
    }

    // Деструктор
    ~Piece() {
        std::cout << "Деструктор: удалена фигура '" << name << "'" << std::endl;
    }

    // Метод для вывода (демонстрация)
    void show() const {
        std::cout << "Фигура: " << name
            << ", Позиция: (" << row << "," << col << ")"
            << ", Цвет: " << (color == Color::White ? "Белый" : "Чёрный")
            << std::endl;
    }
};

int main() {
    // Настройка консоли для русского текста
    _setmode(_fileno(stdout), _O_U16TEXT);
    std::wcout << L"=== Лабораторная работа: класс Piece ===\n\n";

    // 1. Объект по умолчанию
    Piece p1;
    p1.show();

    // 2. Объект с параметрами
    Piece p2('Q', 3, 4, Color::Black);
    p2.show();

    // 3. Копирование
    Piece p3 = p2;
    p3.show();

    // 4. Массив объектов
    std::wcout << L"\n--- Массив из 2 фигур ---\n";
    Piece board[2];
    for (int i = 0; i < 2; ++i) {
        board[i].show();
    }

    // 5. Динамический объект
    std::wcout << L"\n--- Динамическая фигура ---\n";
    Piece* pDynamic = new Piece('K', 7, 4, Color::White);
    pDynamic->show();
    delete pDynamic;  // Явный вызов деструктора

    // 6. Неявный деструктор в блоке
    {
        std::wcout << L"\n--- Временная фигура в блоке ---\n";
        Piece temp('R', 0, 0, Color::White);
        temp.show();
    } // ← деструктор автоматически

    std::wcout << L"\n=== Конец программы ===\n";
    return 0;
}