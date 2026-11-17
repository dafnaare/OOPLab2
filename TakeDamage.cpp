#include "Unit.h"
#include "Base.h"

void Unit::takeDamage(int amount, int defenseBonus) {
    int totalArmor = std::max(0, armor.getValue() + defenseBonus);
    int finalDamage = std::max(0, amount - totalArmor);
    health.decrease(finalDamage);

    if (health.isZero() && ownerBase != nullptr) {
        ownerBase->onUnitDeath(this);
    }
}