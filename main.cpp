#include <iostream>
#include <limits>
#include <memory>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "Color.h"
#include "Unit.h"
#include "GameField.h"
#include "UnitFactory.h"
#include "Base.h"

int getIntInput() {
    int value;
    while (!((std::cin >> value) && (value >= 0))) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка ввода. Введите неотрицательное целое число: ";
    }
    return value;
}

int inputXY(int maxCoord) {
    int coord = getIntInput();
    while (!(coord >= 0 && coord <= maxCoord)) {
        std::cout << "Ошибка ввода. Введите координату от 0 до " << maxCoord << ": ";
        coord = getIntInput();
    }
    return coord;
}

void updateField(GameField& field) {
    system("cls");
    std::cout << "========================================\n";
    std::cout << "           ТЕКУЩЕЕ ПОЛЕ\n";
    std::cout << "========================================\n";
    field.print();
}

void runDemoScenarios() {
    system("cls");
    std::cout << "========================================\n";
    std::cout << "   ДЕМОНСТРАЦИОННЫЕ СЦЕНАРИИ\n";
    std::cout << "========================================\n\n";

    YourArmyFactory myFactoryDemo;
    EnemiesFactory enemyFactoryDemo;

    std::cout << "СЦЕНАРИЙ 1: Создание поля и баз\n\n";

    GameField field(8, 8, 20);

    std::unique_ptr<Base> myBase = std::make_unique<Base>(0, 7, 9);
    std::unique_ptr<Base> enemyBase = std::make_unique<Base>(7, 0, 12);

    field.placeBase(myBase.get(), myBase->getX(), myBase->getY());
    field.placeBase(enemyBase.get(), enemyBase->getX(), enemyBase->getY());

    std::cout << "Создано поле 8x8. Размещены две базы (Синяя и Красная).\n\n";

    std::cout << "Создаем юнитов через базу...\n";
    auto unit1 = myBase->createUnit(myFactoryDemo, 1);
    auto unit2 = myBase->createUnit(myFactoryDemo, 3);

    field.addUnit(std::move(unit1), 0, 6);
    field.addUnit(std::move(unit2), 1, 7);

    std::cout << "\nПоле после создания юнитов:\n";
    field.print();

    std::cout << "\nСценарий 1 завершён. Нажмите Enter...";
    std::cin.get();
    std::cin.get();

    system("cls");
    std::cout << "СЦЕНАРИЙ 2: Взаимодействие с ландшафтом\n\n";


    std::cout << "Показываем информацию о клетках и их влияние на юнитов:\n";

    field.printCellInfo(0, 6);

    std::cout << "\n(В реальной игре игрок видит эти эффекты в меню хода юнита)\n";

    std::cout << "\nСценарий 2 завершён. Нажмите Enter...";
    std::cin.get();

    system("cls");
    std::cout << "СЦЕНАРИЙ 3: Нейтральные объекты и перегрузка операторов\n\n";

    std::cout << "Создаем сундук в (0,5) и ловушку в (1,6)...\n";
    field.placeObject(std::make_unique<Chest>(), 0, 5);
    field.placeObject(std::make_unique<Trap>(), 1, 6);

    Unit* testUnit = field.getUnitAt(0, 6);

    if (testUnit) {
        std::cout << "\nЮнит наступает на сундук (перегрузка +)...\n";
        Chest testChest;
        *testUnit + testChest;

        std::cout << "\nЮнит наступает на ловушку (перегрузка -)...\n";
        Trap testTrap;
        *testUnit - testTrap;
    }

    std::cout << "\nСценарий 3 завершён. Нажмите Enter...";
    std::cin.get();

    system("cls");
    std::cout << "СЦЕНАРИЙ 4: Уведомление базы о смерти юнита (Наблюдатель)\n\n";

    std::cout << "База знает о своих юнитах. При смерти юнита она получает уведомление.\n";
    std::cout << "Сейчас у базы юнитов: " << myBase->getCurrentUnitsCount() << "\n";

    if (testUnit) {
        std::cout << "\nНаносим юниту смертельный урон (200 единиц)...\n";
        testUnit->takeDamage(200, 0);

        std::cout << "\nПосле урона:\n";
        field.removeDeadUnits();
        field.print();
    }

    std::cout << "\nСценарий 4 завершён. Нажмите Enter...";
    std::cin.get();

    system("cls");
    std::cout << "СЦЕНАРИЙ 5: Паттерн Стратегия (Алтарь)\n\n";

    std::cout << "Создаем Алтарь и ставим на него юнита (Лучника)...\n";
    field.placeObject(std::make_unique<Altar>(), 2, 6);

    Unit* archer = field.getUnitAt(1, 7);

    if (archer) {
        std::cout << "Лучник взаимодействует с Алтарём (должен получить лечение, т.к. тип Archer)...\n";
        Altar testAltar;
        *archer* testAltar;
    }

    std::cout << "\nСценарий 5 завершён. Нажмите Enter...";
    std::cin.get();

    system("cls");
    std::cout << "Все демо-сценарии выполнены!\n";
    std::cout << "Нажмите Enter для возврата в меню...";
    std::cin.get();
}

void baseMenu(Base& base, const ArmyFactory& factory, GameField& field, bool& isPlayerTurn) {
    int choice = 0;
    while (choice != 3) {
        system("cls");
        field.print();
        std::cout << "\n";
        base.printInfo();
        std::cout << "\n--- МЕНЮ БАЗЫ ---\n";
        std::cout << "1. Создать юнита\n";
        std::cout << "2. Вывести информацию о юнитах этой базы\n";
        std::cout << "3. Назад\n";
        std::cout << "Выберите действие: ";
        choice = getIntInput();

        if (choice == 1) {
            int type, x, y;
            std::cout << "\nТипы юнитов:\n1.Мечник(S)\n2.Копейщик(P)\n3.Легкий лучник(L)\n4.Тяжелый лучник(H)\n5.Катафракт(C)\n6.Легкий наездник(R)\n";
            std::cout << "Ваш выбор: "; type = getIntInput();

            auto unit = base.createUnit(factory, type);
            if (!unit) {
                system("pause");
                continue;
            }

            std::cout << "Введите X для размещения: "; x = inputXY(field.getWidth() - 1);
            std::cout << "Введите Y для размещения: "; y = inputXY(field.getHeight() - 1);

            try {
                field.addUnit(std::move(unit), x, y);
                std::cout << "Юнит размещён на поле!\n";
                choice = 3;
                isPlayerTurn = !isPlayerTurn;
            }
            catch (const std::exception& e) {
                std::cout << "Ошибка размещения: " << e.what() << "\n";
            }
            system("pause");
        }
        else if (choice == 2) {
            std::cout << "\n--- Юниты этой базы ---\n";
            int count = 0;
            for (auto it = field.begin(); it != field.end(); ++it) {
                if (it->unit->getOwnerBase() == &base) {
                    it->unit->printInfo();
                    std::cout << " на (" << it->x << "," << it->y << ")\n";
                    count++;
                }
            }
            if (count == 0) std::cout << "Нет юнитов.\n";
            system("pause");
        }
    }
}

void unitTurnMenu(GameField& field, int& x, int& y) {
    Unit* unit = field.getUnitAt(x, y);
    if (!unit) return;

    int choice = 0;
    while (choice != 4 && unit->getMovementPoints() > 0) {
        system("cls");
        field.print();
        
        std::cout << "\n========================================\n";
        std::cout << "   ХОД ЮНИТА: " << unit->getName() << "\n";
        std::cout << "========================================\n";
        
        std::cout << "Ваш юнит: ";
        unit->printInfo();
        std::cout << "\n";

        field.printCellInfo(x, y);

        std::cout << "Очков перемещения: " << unit->getMovementPoints() << "\n";
        std::cout << "\n1. Перемещение (на 1 клетку)\n";
        std::cout << "2. Атака (пока недоступно)\n";
        std::cout << "3. Вывести информацию о юнитах\n";
        std::cout << "4. Завершить ход\n";
        std::cout << "Ваш выбор: ";
        choice = getIntInput();

        if (choice == 1) {
            std::cout << "Введите направление (1-Верх, 2-Право, 3-Низ, 4-Лево): ";
            int dir = getIntInput();
            int nx = x, ny = y;

            if (dir == 1) ny--;
            else if (dir == 2) nx++;
            else if (dir == 3) ny++;
            else if (dir == 4) nx--;

            try {
                field.gameMoveUnit(x, y, nx, ny);
                x = nx; y = ny;
                std::cout << "Юнит переместился.\n";
                field.removeDeadUnits();
                if (unit->getMovementPoints() <= 0) {
                    setColor(12, 0);
                    std::cout << "У юнита закончились очки перемещения. Ход переходит к сопернику!\n";
                    setColor(7, 0);
                }
            }
            catch (const std::exception& e) {
                std::cout << "Ошибка перемещения: " << e.what() << "\n";
            }
            system("pause");
        }
        else if (choice == 2) {
            std::cout << "Атака пока не реализована. Ход завершён.\n";
            field.removeDeadUnits();
            system("pause");
            choice = 4;
        }
        else if (choice == 3) {
            std::cout << "\n--- Все юниты на поле ---\n";
            for (auto it = field.begin(); it != field.end(); ++it) {
                it->unit->printInfo();
                std::cout << " на (" << it->x << "," << it->y << ")\n";
            }
            system("pause");
        }
    }
    unit->resetMovement();
}

void playGame(GameField& field, YourArmyFactory& myFactory, EnemiesFactory& enemyFactory, Base& myBase, Base& enemyBase) {
    bool isPlayerTurn = true;
    int choice = 0;

    while (choice != 4) {
        updateField(field);

        if (isPlayerTurn) setColor(9, 0);
        else setColor(12, 0);

        std::cout << "\nХОД: " << (isPlayerTurn ? "СИНИХ" : "КРАСНЫХ") << "\n";
        setColor(7, 0);

        std::cout << "1. База\n";
        std::cout << "2. Выбрать юнита для хода\n";
        std::cout << "3. Информация о нейтральных объектах\n";
        std::cout << "4. Выход (в главное меню)\n";
        std::cout << "Выберите действие: ";
        choice = getIntInput();

        if (choice == 1) {
            if (isPlayerTurn) baseMenu(myBase, myFactory, field, isPlayerTurn);
            else baseMenu(enemyBase, enemyFactory, field, isPlayerTurn);
        }
        else if (choice == 2) {
            int x, y;
            std::cout << "Введите координаты юнита: ";
            x = inputXY(field.getWidth() - 1); y = inputXY(field.getHeight() - 1);

            Unit* u = field.getUnitAt(x, y);
            if (!u) {
                std::cout << "Там нет юнита!\n";
                system("pause");
            }
            else if (u->getOwnerBase() != (isPlayerTurn ? &myBase : &enemyBase)) {
                std::cout << "Это не ваш юнит!\n";
                system("pause");
            }
            else {
                unitTurnMenu(field, x, y);
                isPlayerTurn = !isPlayerTurn;
            }
        }
        else if (choice == 3) {
            int x, y;
            std::cout << "Введите координаты для проверки: ";
            x = inputXY(field.getWidth() - 1); y = inputXY(field.getHeight() - 1);
            field.printObjectInfo(x, y);
            system("pause");
        }
    }
}

void gameLoop(GameField& field, YourArmyFactory& myFactory, EnemiesFactory& enemyFactory) {
    int choice = 0;

    std::unique_ptr<Base> myBase = std::make_unique<Base>(0, field.getHeight() - 1, 9);
    std::unique_ptr<Base> enemyBase = std::make_unique<Base>(field.getWidth() - 1, 0, 12);

    field.placeBase(myBase.get(), myBase->getX(), myBase->getY());
    field.placeBase(enemyBase.get(), enemyBase->getX(), enemyBase->getY());

    while (choice != 10) {
        updateField(field);

        std::cout << "\n========================================\n";
        std::cout << "           ЧТО ХОТИТЕ СДЕЛАТЬ?\n";
        std::cout << "========================================\n";
        std::cout << "1. Добавить своего юнита (синий)\n";
        std::cout << "2. Добавить врага (красный)\n";
        std::cout << "3. Переместить юнита (вручную)\n";
        std::cout << "4. Удалить юнита\n";
        std::cout << "5. Показать подробную информацию обо всех юнитах\n";
        std::cout << "6. Скопировать поле (Демонстрация глубокого копирования)\n";
        std::cout << "7. Запустить сценарии использования\n";
        std::cout << "8. Добавить нейтральные объекты на поле\n";
        std::cout << "9. Начать игру (пошаговый режим)\n";
        std::cout << "10. Выход\n";
        std::cout << "Выберите действие: ";

        choice = getIntInput();

        switch (choice) {
        case 1:
        case 2: {
            int type, x, y;
            std::cout << "\nВыберите тип юнита:\n";
            std::cout << "1. Мечник (S)\n2. Копейщик (P)\n3. Легкий лучник (L)\n4. Тяжелый лучник (H)\n5. Катафракт (C)\n6. Легкий наездник (R)\n";
            std::cout << "Ваш выбор: ";
            type = getIntInput();

            if (type < 1 || type > 6) {
                std::cout << "Неверный тип юнита!\n";
            }
            else {
                std::cout << "Введите координату X: ";
                x = inputXY(field.getWidth() - 1);
                std::cout << "Введите координату Y: ";
                y = inputXY(field.getHeight() - 1);

                Base* targetBase = (choice == 1) ? myBase.get() : enemyBase.get();
                const ArmyFactory& factory = (choice == 1) ? static_cast<const ArmyFactory&>(myFactory) : static_cast<const ArmyFactory&>(enemyFactory);

                try {
                    auto unit = targetBase->createUnit(factory, type);

                    if (unit) {
                        field.addUnit(std::move(unit), x, y);
                    }
                }
                catch (const std::exception& e) {
                    std::cout << "Ошибка: " << e.what() << "\n";
                }
            }
            field.removeDeadUnits();
            system("pause");
            break;
        }
        case 3: {
            int fx, fy, tx, ty;
            std::cout << "\nОткуда переместить? X и Y: ";
            fx = inputXY(field.getWidth() - 1); fy = inputXY(field.getHeight() - 1);
            std::cout << "Куда переместить? X и Y: ";
            tx = inputXY(field.getWidth() - 1); ty = inputXY(field.getHeight() - 1);
            try {
                field.moveUnit(fx, fy, tx, ty);
                std::cout << "Юнит перемещен!\n";
            }
            catch (const std::exception& e) {
                std::cout << "Ошибка: " << e.what() << "\n";
            }
            field.removeDeadUnits();
            system("pause");
            break;
        }
        case 4: {
            int x, y;
            std::cout << "\nВведите координаты юнита для удаления X и Y: ";
            x = inputXY(field.getWidth() - 1); y = inputXY(field.getHeight() - 1);
            field.removeUnit(x, y);
            std::cout << "Юнит удален.\n";
            system("pause");
            break;
        }
        case 5: {
            std::cout << "\n--- Юниты на поле ---\n";
            for (auto it = field.begin(); it != field.end(); ++it) {
                std::cout << "Клетка (" << it->x << "," << it->y << "): ";
                it->unit->printInfo();
                std::cout << "\n";
            }
            system("pause");
            break;
        }
        case 6: {
            std::cout << "\nСоздаем копию поля...\n";
            GameField copyField = field;
            field.removeUnit(0, 0);
            std::cout << "\n=== ОРИГИНАЛ ===\n"; field.print();
            std::cout << "\n=== КОПИЯ ===\n"; copyField.print();
            system("pause");
            break;
        }
        case 7: {
            runDemoScenarios();
            break;
        }
        case 8: {
            int chests, traps, altars, springs;
            int maxPerType = (field.getWidth() * field.getHeight()) / 4 - 1;

            std::cout << "\nМаксимум объектов каждого типа: " << maxPerType << "\n";
            std::cout << "Сколько сундуков создать? "; chests = getIntInput();
            std::cout << "Сколько ловушек создать? "; traps = getIntInput();
            std::cout << "Сколько алтарей создать? "; altars = getIntInput();
            std::cout << "Сколько источников создать? "; springs = getIntInput();

            if (chests > maxPerType || traps > maxPerType || altars > maxPerType || springs > maxPerType) {
                std::cout << "Ошибка: Превышено максимальное количество объектов!\n";
                system("pause");
                break;
            }

            auto placeRandom = [&](std::unique_ptr<NeutralObject> obj) {
                int attempts = 0;
                while (attempts < 1000) {
                    int rx = rand() % field.getWidth();
                    int ry = rand() % field.getHeight();
                    try {
                        field.placeObject(std::move(obj), rx, ry);
                        return true;
                    }
                    catch (...) {

                    }
                    attempts++;
                }
                return false;
                };

            for (int i = 0; i < chests; ++i) placeRandom(std::make_unique<Chest>());
            for (int i = 0; i < traps; ++i) placeRandom(std::make_unique<Trap>());
            for (int i = 0; i < altars; ++i) placeRandom(std::make_unique<Altar>());
            for (int i = 0; i < springs; ++i) placeRandom(std::make_unique<Spring>());

            std::cout << "Объекты успешно добавлены на поле!\n";
            system("pause");
            break;
        }
        case 9: {
            playGame(field, myFactory, enemyFactory, *myBase, *enemyBase);
            break;
        }
        case 10:
            std::cout << "Выход из программы...\n";
            break;
        default:
            std::cout << "Неверный выбор. Попробуйте снова.\n";
            system("pause");
            break;
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    srand(time(0));

    system("cls");
    std::cout << "========================================\n";
    std::cout << "      ДОБРО ПОЖАЛОВАТЬ В ИГРУ\n";
    std::cout << "========================================\n";

    int w, h, maxU;
    std::cout << "Введите ширину поля: "; w = getIntInput();
    std::cout << "Введите высоту поля: "; h = getIntInput();
    std::cout << "Введите максимальное количество юнитов: "; maxU = getIntInput();

    while (!(w > 0 && h > 0 && maxU >= 0 && maxU <= (w * h))) {
        setColor(12, 0);
        std::cout << "Ошибка ввода (возможно, одна из характеристик <= 0 или максимум юнитов больше, чем мест на поле)\n";
        setColor(7, 0);
        std::cout << "Введите ширину поля: "; w = getIntInput();
        std::cout << "Введите высоту поля: "; h = getIntInput();
        std::cout << "Введите максимальное количество юнитов: "; maxU = getIntInput();
    }

    GameField field(w, h, maxU);
    YourArmyFactory myFactory;
    EnemiesFactory enemyFactory;

    gameLoop(field, myFactory, enemyFactory);

    return 0;
}