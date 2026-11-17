#pragma once
#include "Unit.h"
#include "Strategy.h"

class NeutralObject {
public:
    virtual ~NeutralObject() = default;
    virtual char getSymbol() const = 0;
    virtual int getColor() const = 0;
    virtual std::string getName() const = 0;
    virtual void interact(Unit& unit, int defenseBonus) = 0;
};

class Chest : public NeutralObject {
public:
    char getSymbol() const override { return '$'; }
    int getColor() const override { return 14; }
    std::string getName() const override { return "Сундук"; }
    void interact(Unit& unit, int defenseBonus) override {
        setColor(getColor(), 0);
        std::cout << "Сундук открыт! Получено 100 ресурсов.\n";
        setColor(7, 0);
    }
};

class Trap : public NeutralObject {
public:
    char getSymbol() const override { return '#'; }
    int getColor() const override { return 12; }
    std::string getName() const override { return "Ловушка"; }
    void interact(Unit& unit, int defenseBonus) override {
        setColor(getColor(), 0);
        std::cout << "Ловушка! Юнит получает 20 урона.\n";
        setColor(7, 0);
        unit.takeDamage(20, defenseBonus);
    }
};

class Altar : public NeutralObject {
private:
    InteractionStrategy* currentStrategy = nullptr;

    AttackBoostStrategy attackStrategy;
    HealthRestoreStrategy healthStrategy;
    ArmorBoostStrategy armorStrategy;

public:
    char getSymbol() const override { return (char)30; }
    int getColor() const override { return 13; }
    std::string getName() const override { return "Алтарь силы"; }

    void interact(Unit& unit, int defenseBonus) override {
        setColor(getColor(), 0);
        std::cout << "Алтарь активирован! ";
        setColor(7, 0);

        std::string type = unit.getType();

        if (type == "Infantry") {
            currentStrategy = &attackStrategy;
        }
        else if (type == "Archer") {
            currentStrategy = &healthStrategy;
        }
        else if (type == "Cavalry") {
            currentStrategy = &armorStrategy;
        }
        else {
            currentStrategy = &attackStrategy;
        }

        if (currentStrategy) {
            currentStrategy->apply(unit);
        }
    }
};

class Spring : public NeutralObject {
public:
    char getSymbol() const override { return (char)31; }
    int getColor() const override { return 11; }
    std::string getName() const override { return "Источник жизни"; }
    void interact(Unit& unit, int defenseBonus) override {
        setColor(getColor(), 0);
        std::cout << "Источник жизни! Юнит полностью восстановил здоровье.\n";
        setColor(7, 0);
        unit.heal(9999);
    }
};