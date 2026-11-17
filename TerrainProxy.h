#pragma once
#include "Terrain.h"

class TerrainProxy : public Terrain {
private:
    Terrain* realTerrain;
public:
    TerrainProxy(Terrain* t) : realTerrain(t) {}

    char getSymbol() const override { return realTerrain->getSymbol(); }
    int getColor() const override { return realTerrain->getColor(); }

    bool isPassable(const Unit& unit) const override {
        if (unit.getIsStunned()) return false;
        return realTerrain->isPassable(unit);
    }

    double getSpeedModifier(const Unit& unit) const override {
        return realTerrain->getSpeedModifier(unit);
    }

    double getDefenseBonus(const Unit& unit) const override {
        return realTerrain->getDefenseBonus(unit);
    }
};