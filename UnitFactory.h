#pragma once
#include <memory>
#include "Unit.h"

class ArmyFactory {
public:
    virtual ~ArmyFactory() = default;
    virtual std::unique_ptr<Unit> createSwordsman() const = 0;
    virtual std::unique_ptr<Unit> createPikeman() const = 0;

    virtual std::unique_ptr<Unit> createLightArcher() const = 0;
    virtual std::unique_ptr<Unit> createHeavyArcher() const = 0;

    virtual std::unique_ptr<Unit> createCataphract() const = 0;
    virtual std::unique_ptr<Unit> createRider() const = 0;
    virtual int getColor() const = 0;
};

class YourArmyFactory : public ArmyFactory {
public:
    std::unique_ptr<Unit> createSwordsman() const override { return std::make_unique<YourSwordsman>(); }
    std::unique_ptr<Unit> createPikeman() const override { return std::make_unique<YourPikeman>(); }

    std::unique_ptr<Unit> createLightArcher() const override { return std::make_unique<YourLightArcher>(); }
    std::unique_ptr<Unit> createHeavyArcher() const override { return std::make_unique<YourHeavyArcher>(); }

    std::unique_ptr<Unit> createCataphract() const override { return std::make_unique<YourCataphract>(); }
    std::unique_ptr<Unit> createRider() const override { return std::make_unique<YourRider>(); }
    int getColor() const override { return 9; }
};

class EnemiesFactory : public ArmyFactory {
public:
    std::unique_ptr<Unit> createSwordsman() const override { return std::make_unique<EnemySwordsman>(); }
    std::unique_ptr<Unit> createPikeman() const override { return std::make_unique<EnemyPikeman>(); }

    std::unique_ptr<Unit> createLightArcher() const override { return std::make_unique<EnemyLightArcher>(); }
    std::unique_ptr<Unit> createHeavyArcher() const override { return std::make_unique<EnemyHeavyArcher>(); }

    std::unique_ptr<Unit> createCataphract() const override { return std::make_unique<EnemyCataphract>(); }
    std::unique_ptr<Unit> createRider() const override { return std::make_unique<EnemyRider>(); }
    int getColor() const override { return 12; }
};