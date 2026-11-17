#pragma once
#include "Unit.h"

class InteractionStrategy {
public:
    virtual ~InteractionStrategy() = default;
    virtual void apply(Unit& unit) = 0;
};

class AttackBoostStrategy : public InteractionStrategy {
public:
    void apply(Unit& unit) override {
        std::cout << "Стратегия: +5 к атаке.\n";
        unit.addAttack(5);
    }
};

class HealthRestoreStrategy : public InteractionStrategy {
public:
    void apply(Unit& unit) override {
        std::cout << "Стратегия: +20 к здоровью.\n";
        unit.heal(20);
    }
};

class ArmorBoostStrategy : public InteractionStrategy {
public:
    void apply(Unit& unit) override {
        std::cout << "Стратегия: +5 к броне.\n";
        unit.addArmor(5);
    }
};