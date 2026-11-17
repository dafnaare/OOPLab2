#pragma once
#include "Unit.h"
#include "NeutralObject.h"

void operator+(Unit& unit, Chest& chest) {
    chest.interact(unit, 0);
}

void operator-(Unit& unit, Trap& trap) {
    trap.interact(unit, 0);
}

void operator*(Unit& unit, Altar& altar) {
    altar.interact(unit, 0);
}

void operator/(Unit& unit, Spring& spring) {
    spring.interact(unit, 0);
}