#include <string>
#include <iostream>
#include <io.h>
#include <fcntl.h>


enum Level { Easy, Medium, Hard }; // Перечисление для уровня

class MathAnalysis
{
private:
    std::string name;  // Название
    Level level;      // Уровень
    int topics;       // Количество тем

public:
    // Конструктор без параметров
    MathAnalysis()
    {
        this->name = "Math Basics";
        this->level = Level::Easy;;
        this->topics = 0;

        std::cout << "Конструктор без параметров" << std::endl;
    }

    // Конструктор с параметрами - с использованием списка инициализации
    MathAnalysis(std::string n, Level l, int t) : name(n), level(l), topics(t)
    {
        std::cout << "Конструктор с параметрами" << std::endl;
    }

    // Конструктор копирования
    MathAnalysis(const MathAnalysis& other)
    {
        name = other.name;
        level = other.level;
        topics = other.topics;
        std::cout << "Конструктор копирования" << std::endl;
    }

    // Деструктор
    ~MathAnalysis()
    {
        std::cout << "Деструктор: " << name << std::endl;
    }

    // Показать информацию
    void show()
    {
        std::cout << "Название: " << name << ", Уровень: ";
        if (level == Easy) std::cout << "Easy";
        else if (level == Medium) std::cout << "Medium";
        else std::cout << "Hard";
        std::cout << ", Тем: " << topics << std::endl;
    }
};

int main()
{
    setlocale(LC_ALL, "RUS");
    std::cout << "=== Начало программы ===" << std::endl;

    // Создаём объект по умолчанию
    MathAnalysis obj1;
    obj1.show();

    // Создаём объект с параметрами
    MathAnalysis obj2("Calculus", Medium, 10);
    obj2.show();

    // Копируем объект
    MathAnalysis obj3 = obj2;
    obj3.show();

    // Массив объектов
    MathAnalysis arr[2]; // Два объекта по умолчанию
    for (int i = 0; i < 2; i++)
    {
        arr[i].show();
    }

    // Динамический объект
    MathAnalysis* obj4 = new MathAnalysis("Advanced Math", Hard, 15);
    obj4->show();
    delete obj4; // Удаляем динамический объект

    std::cout << "Конец программы" << std::endl;
    return 0;
}