#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include "Color.h"
#include "Unit.h"
#include "UnitFactory.h"

class Base {
private:
    int x, y;
    int teamColor;
    int health;
    int maxHealth;
    int maxUnits;
    int defenseLevel;
    int resources;
    int currentUnitsCount;

public:
    Base(int startX, int startY, int color)
        : x(startX), y(startY), teamColor(color),
        health(1000), maxHealth(1000),
        maxUnits(10), defenseLevel(10),
        resources(0), currentUnitsCount(0) {
    }

    int getX() const { return x; }
    int getY() const { return y; }
    int getColor() const { return teamColor; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getResources() const { return resources; }
    int getCurrentUnitsCount() const { return currentUnitsCount; }
    int getMaxUnits() const { return maxUnits; }

    void printInfo() const {
        setColor(teamColor, 0);
        std::cout << "--- БАЗА (Команда " << (teamColor == 9 ? "Свои" : "Враги") << ") ---\n";
        std::cout << "HP: " << health << "/" << maxHealth
            << " | Ресурсы: " << resources
            << " | Юниты: " << currentUnitsCount << "/" << maxUnits
            << " | Защита: " << defenseLevel << "\n";
        setColor(7, 0);
    }


    std::unique_ptr<Unit> createUnit(const ArmyFactory& factory, int type) {
        if (currentUnitsCount >= maxUnits) {
            std::cout << "Ошибка: Достигнут лимит юнитов для этой базы!\n";
            return nullptr;
        }

        std::unique_ptr<Unit> newUnit = nullptr;

        switch (type) {
        case 1: newUnit = factory.createSwordsman(); break;
        case 2: newUnit = factory.createPikeman(); break;
        case 3: newUnit = factory.createLightArcher(); break;
        case 4: newUnit = factory.createHeavyArcher(); break;
        case 5: newUnit = factory.createCataphract(); break;
        case 6: newUnit = factory.createRider(); break;
        default:
            std::cout << "Ошибка: Неизвестный тип юнита!\n";
            return nullptr;
        }

        if (newUnit) {
            newUnit->setOwnerBase(this);
            currentUnitsCount++;
            setColor(teamColor, 0);
            std::cout << "Юнит [" << newUnit->getName() << "] создан базой и добавлен в учёт.\n";
            setColor(7, 0);
        }
        return newUnit;
    }


    void addResources(int amount) {
        resources += amount;
        std::cout << "База получила " << amount << " ресурсов. Итого: " << resources << "\n";
    }

    bool spendResources(int amount) {
        if (resources >= amount) {
            resources -= amount;
            return true;
        }
        return false;
    }


    void takeDamage(int incomingDamage) {
        int reduced = std::max(0, incomingDamage - defenseLevel);
        health = std::max(0, health - reduced);

        setColor(12, 0);
        std::cout << "База получила " << reduced << " урона! Осталось HP: " << health << "\n";
        setColor(7, 0);

        if (health == 0) {
            setColor(12, 0);
            std::cout << "!!! БАЗА УНИЧТОЖЕНА! Команда " << (teamColor == 9 ? "Свои" : "Враги") << " проиграла. !!!\n";
            setColor(7, 0);
        }
    }

    void onUnitDeath(Unit* deadUnit) {
        setColor(teamColor, 0);
        std::cout << "База " << (teamColor == 9 ? "СИНИХ" : "КРАСНЫХ") << " получила уведомление: юнит ["
            << deadUnit->getName() << "] погиб.\n";
        setColor(7, 0);
        if (currentUnitsCount > 0) {
            currentUnitsCount--;
        }
    }
};