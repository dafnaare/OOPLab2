#pragma once
#include <iostream>
#include <cmath>
#include "Unit.h"

class Terrain {
public:
    virtual ~Terrain() = default;
    virtual char getSymbol() const = 0;
    virtual int getColor() const = 0;
    virtual bool isPassable(const Unit& unit) const = 0;
    virtual double getSpeedModifier(const Unit& unit) const = 0;
    virtual double getDefenseBonus(const Unit& unit) const = 0;
};

class Plains : public Terrain {
public:
    char getSymbol() const override { return '"'; }
    int getColor() const override { return 10; }
    bool isPassable(const Unit&) const override { return true; }
    double getSpeedModifier(const Unit&) const override { return 1.0; }
    double getDefenseBonus(const Unit&) const override { return 0.0; }
};

class Forest : public Terrain {
public:
    char getSymbol() const override { return (char)05; }
    int getColor() const override { return 2; }
    bool isPassable(const Unit& unit) const override {
        return unit.getType() != "Cavalry";
    }
    double getSpeedModifier(const Unit& unit) const override {
        return (unit.getType() == "Infantry") ? 0.3 : 0.5;
    }
    double getDefenseBonus(const Unit&) const override { return 5.0; }
};

class Swamp : public Terrain {
public:
    char getSymbol() const override { return '~'; }
    int getColor() const override { return 3; }
    bool isPassable(const Unit&) const override { return true; }
    double getSpeedModifier(const Unit&) const override { return 0.3; }
    double getDefenseBonus(const Unit&) const override { return -2.0; }
};