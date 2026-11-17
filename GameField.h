#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <cstdlib>
#include "Color.h"
#include "Base.h"
#include "Unit.h"
#include "UnitFactory.h"
#include "TerrainProxy.h"
#include "NeutralObject.h"
#include "Operators.h"

class GameField {
public:
    struct Cell {
        std::unique_ptr<Unit> unit;

        std::unique_ptr<TerrainProxy> terrain;
        std::unique_ptr<NeutralObject> object;

        Base* base = nullptr;

        int x = -1;
        int y = -1;

        Cell() = default;

        Cell(const Cell&) = delete;
        Cell& operator=(const Cell&) = delete;

        Cell(Cell&&) noexcept = default;
        Cell& operator=(Cell&&) noexcept = default;

        bool hasUnit() const { return unit != nullptr; }

        void print() const {
            if (unit) {
                unit->printInfo();
                setColor(7, 0);
                std::cout << " at (" << x << "," << y << ")";
            }
        }
    };

    class Iterator {
    private:
        GameField* field;
        int curX, curY;

        void skipEmpty() {
            while (curY < field->height) {
                if (curX >= field->width) {
                    curX = 0;
                    curY++;
                    continue;
                }
                if (field->grid[curY][curX].hasUnit()) break;
                curX++;
            }
        }

    public:
        Iterator(GameField* f, int x, int y) : field(f), curX(x), curY(y) {
            skipEmpty();
        }

        Cell& operator*() const { return field->grid[curY][curX]; }
        Cell* operator->() const { return &field->grid[curY][curX]; }

        Iterator& operator++() {
            curX++;
            skipEmpty();
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return curX != other.curX || curY != other.curY;
        }
    };

private:
    int width = 5, height = 5, maxUnits = 6, currentUnitCount = 0;
    std::vector<std::vector<Cell>> grid;

public:
    GameField(int w, int h, int maxU)
        : width(w), height(h), maxUnits(maxU), currentUnitCount(0) {

        if (w <= 0) throw std::invalid_argument("Ширина поля должна быть положительной");
        if (h <= 0) throw std::invalid_argument("Высота поля должна быть положительной");
        if (maxU <= 0) throw std::invalid_argument("Максимум юнитов должен быть положительным");
        if (maxU > w * h) throw std::invalid_argument("Максимум юнитов не может превышать размер поля");

        grid.reserve(height);
        for (int y = 0; y < height; ++y) {
            std::vector<Cell> row;
            row.reserve(width);
            for (int x = 0; x < width; ++x) {
                row.emplace_back();
                row.back().x = x;
                row.back().y = y;

                int r = rand() % 10;
                if (r < 6)      row.back().terrain = std::make_unique<TerrainProxy>(new Plains());
                else if (r < 8) row.back().terrain = std::make_unique<TerrainProxy>(new Forest());
                else            row.back().terrain = std::make_unique<TerrainProxy>(new Swamp());
            }
            grid.push_back(std::move(row));
        }
    }

    ~GameField() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }


    GameField(const GameField& other)
        : width(other.width), height(other.height), maxUnits(other.maxUnits), currentUnitCount(0) {

        grid.reserve(height);
        for (int y = 0; y < height; ++y) {
            std::vector<Cell> row;
            row.reserve(width);
            for (int x = 0; x < width; ++x) {
                row.emplace_back();
                row.back().x = x;
                row.back().y = y;
                if (other.grid[y][x].hasUnit()) {
                    row.back().unit = other.grid[y][x].unit->clone();
                    currentUnitCount++;
                }
            }
            grid.push_back(std::move(row));
        }
    }

    GameField& operator=(const GameField& other) {
        if (this != &other) {
            GameField temp(other);
            std::swap(width, temp.width);
            std::swap(height, temp.height);
            std::swap(maxUnits, temp.maxUnits);
            std::swap(currentUnitCount, temp.currentUnitCount);
            std::swap(grid, temp.grid);
        }
        return *this;
    }

    GameField(GameField&& other) noexcept
        : width(other.width), height(other.height), maxUnits(other.maxUnits),
        currentUnitCount(other.currentUnitCount), grid(std::move(other.grid)) {
        other.width = 0;
        other.height = 0;
        other.currentUnitCount = 0;
    }

    GameField& operator=(GameField&& other) noexcept {
        if (this != &other) {
            width = other.width;
            height = other.height;
            maxUnits = other.maxUnits;
            currentUnitCount = other.currentUnitCount;
            grid = std::move(other.grid);
            other.width = 0;
            other.height = 0;
            other.currentUnitCount = 0;
        }
        return *this;
    }


    void addUnit(std::unique_ptr<Unit> unit, int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height)
            throw std::out_of_range("Координаты выходят за пределы поля");
        if (currentUnitCount >= maxUnits)
            throw std::runtime_error("Достигнут лимит юнитов на поле");
        if (grid[y][x].hasUnit())
            throw std::runtime_error("Клетка уже занята");

        if (grid[y][x].terrain && !grid[y][x].terrain->isPassable(*unit)) {
            throw std::runtime_error("Этот юнит не может находиться на этой местности!");
        }

        if (grid[y][x].base) {
            Base* targetBase = grid[y][x].base;
            if (targetBase->getColor() != unit->getColor()) {
                throw std::runtime_error("Нельзя заходить на вражескую базу!");
            }
        }

        grid[y][x].unit = std::move(unit);
        currentUnitCount++;
    }

    void placeObject(std::unique_ptr<NeutralObject> obj, int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height)
            throw std::out_of_range("Координаты вне поля");
        if (grid[y][x].object)
            throw std::runtime_error("Здесь уже есть объект");
        grid[y][x].object = std::move(obj);
    }

    void placeBase(Base* basePtr, int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height)
            throw std::out_of_range("Координаты базы вне поля");
        grid[y][x].base = basePtr;
    }

    void removeUnit(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        if (grid[y][x].hasUnit()) {
            grid[y][x].unit.reset();
            currentUnitCount--;
        }
    }

    void removeDeadUnits() {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (grid[y][x].hasUnit() && grid[y][x].unit->getHealth() == 0) {
                    grid[y][x].unit.reset();
                    currentUnitCount--;
                }
            }
        }
    }

    void moveUnit(int fromX, int fromY, int toX, int toY) {
        if (fromX < 0 || fromX >= width || fromY < 0 || fromY >= height) return;
        if (toX < 0 || toX >= width || toY < 0 || toY >= height) return;
        if (!grid[fromY][fromX].hasUnit()) return;
        if (grid[toY][toX].hasUnit()) throw std::runtime_error("Целевая клетка занята");

        grid[toY][toX].unit = std::move(grid[fromY][fromX].unit);
    }

    void gameMoveUnit(int fromX, int fromY, int toX, int toY) {
        if (fromX < 0 || fromX >= width || fromY < 0 || fromY >= height)
            throw std::out_of_range("Координаты источника вне поля");
        if (toX < 0 || toX >= width || toY < 0 || toY >= height)
            throw std::out_of_range("Координаты цели вне поля");
        if (!grid[fromY][fromX].hasUnit())
            throw std::runtime_error("В клетке-источнике нет юнита");
        if (grid[toY][toX].hasUnit())
            throw std::runtime_error("Целевая клетка занята или вы ввели не тот символ");

        Unit* unit = grid[fromY][fromX].unit.get();

        if (grid[toY][toX].base) {
            Base* targetBase = grid[toY][toX].base;
            if (targetBase->getColor() != unit->getColor()) {
                throw std::runtime_error("Нельзя заходить на вражескую базу!");
            }
        }

        if (unit->getMovementPoints() <= 0) {
            throw std::runtime_error("У юнита закончились очки движения!");
        }

        if (grid[toY][toX].terrain && !grid[toY][toX].terrain->isPassable(*unit)) {
            throw std::runtime_error("Этот юнит не может пройти через эту местность!");
        }

        if (grid[toY][toX].object) {
            if (auto chest = dynamic_cast<Chest*>(grid[toY][toX].object.get())) {
                *unit + *chest;
            }
            else if (auto trap = dynamic_cast<Trap*>(grid[toY][toX].object.get())) {
                *unit - *trap;
            }
            else if (auto altar = dynamic_cast<Altar*>(grid[toY][toX].object.get())) {
                *unit** altar;
            }
            else if (auto spring = dynamic_cast<Spring*>(grid[toY][toX].object.get())) {
                *unit / *spring;
            }
        }

        grid[toY][toX].unit = std::move(grid[fromY][fromX].unit);

        double modifier = grid[toY][toX].terrain ? grid[toY][toX].terrain->getSpeedModifier(*unit) : 1.0;

        int cost = static_cast<int>(1.0 / modifier);

        if (cost < 1) cost = 1;

        unit->setMovementPoints(unit->getMovementPoints() - cost);
    }

    Unit* getUnitAt(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height) return nullptr;
        return grid[y][x].unit.get();
    }

    void printObjectInfo(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            std::cout << "Координаты вне поля.\n";
            return;
        }

        const Cell& cell = grid[y][x];

        if (cell.object) {
            std::cout << "Объект: ";
            setColor(cell.object->getColor(), 0);
            std::cout << cell.object->getName() << " (" << cell.object->getSymbol() << ")\n";
            setColor(7, 0);
        }
        else {
            std::cout << "На этой клетке нет объектов.\n";
        }
    }

    void printCellInfo(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        const Cell& cell = grid[y][x];

        std::cout << "--- Информация о клетке (" << x << "," << y << ") ---\n";

        if (cell.terrain) {
            std::cout << "Местность: ";
            char sym = cell.terrain->getSymbol();
            if (sym == '"') std::cout << "Равнина";
            else if (sym == 'o') std::cout << "Лес";
            else if (sym == '~') std::cout << "Болото";

            if (cell.unit) {
                double defBonus = cell.terrain->getDefenseBonus(*cell.unit.get());
                double speedMod = cell.terrain->getSpeedModifier(*cell.unit.get());
                std::cout << " (Защита: " << (defBonus >= 0 ? "+" : "") << defBonus
                    << ", Скорость: x" << speedMod << ")";
            }
            std::cout << "\n";
        }

        if (cell.object) {
            std::cout << "Объект: " << cell.object->getName() << " (" << cell.object->getSymbol() << ")\n";
        }
        else {
            std::cout << "Объектов нет.\n";
        }
        std::cout << "----------------------------------------\n";
    }

    void populateArmy(const ArmyFactory& factory, int countPerType) {
        for (int i = 0; i < countPerType; ++i) {
            bool placed = false;
            for (int y = 0; y < height && !placed; ++y) {
                for (int x = 0; x < width && !placed; ++x) {
                    if (!grid[y][x].hasUnit()) {
                        if (i % 6 == 0)      addUnit(factory.createSwordsman(), x, y);
                        else if (i % 6 == 1) addUnit(factory.createPikeman(), x, y);
                        else if (i % 6 == 2) addUnit(factory.createLightArcher(), x, y);
                        else if (i % 6 == 3) addUnit(factory.createHeavyArcher(), x, y);
                        else if (i % 6 == 4) addUnit(factory.createCataphract(), x, y);
                        else                 addUnit(factory.createRider(), x, y);

                        placed = true;
                    }
                }
            }
        }
    }

    Iterator begin() { return Iterator(this, 0, 0); }
    Iterator end() { return Iterator(this, 0, height); }

    void print() const {
        setColor(7, 0);
        std::cout << "--- Игровое поле (" << width << "x" << height << ") ---\n";

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const Cell& cell = grid[y][x];

                if (cell.base) {
                    setColor(cell.base->getColor(), 0);
                    std::cout << " B ";
                }
                else if (cell.hasUnit()) {
                    setColor(cell.unit->getColor(), 0);
                    std::cout << " " << cell.unit->getSign() << " ";
                }
                else if (cell.object) {
                    setColor(cell.object->getColor(), 0);
                    std::cout << " " << cell.object->getSymbol() << " ";
                }
                else if (cell.terrain) {
                    setColor(cell.terrain->getColor(), 0);
                    std::cout << " " << cell.terrain->getSymbol() << " ";
                }
                else {
                    std::cout << " . ";
                }
            }
            std::cout << "\n";
        }

        setColor(7, 0);
        std::cout << "-----------------------------\n";
    }
};