#pragma once
#include "Color.h"
#include <string>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <algorithm>



class Health {
private:
    int value;
    int maxValue;
public:
    Health(int maxVal) : maxValue(maxVal), value(maxVal) {
        if (maxVal <= 0) throw std::invalid_argument("Здоровье не может быть отрицательным");
    }
    void decrease(int amount) {
        value = std::max(0, value - amount);
    }
    void increase(int amount) {
        value = std::min(maxValue, value + amount);
    }
    bool isZero() const { return value == 0; }
    int getValue() const { return value; }
};

class Armor {
private:
    int value;
public:
    Armor(int val) : value(val) {
        if (val < 0) throw std::invalid_argument("Броня не может быть отрицательной");
    }
    int reduceDamage(int incomingDamage) const {
        return std::max(0, incomingDamage - value);
    }
    int getValue() const { return value; }
};

class Attack {
private:
    int damage;
public:
    Attack(int dmg) : damage(dmg) {
        if (dmg < 0) throw std::invalid_argument("Атака не может быть отрицательной");
    }
    int calculateDamage(const Armor& targetArmor) const {
        return targetArmor.reduceDamage(damage);
    }
    int getValue() const { return damage; }
};


class Base;

class Unit {
private:
    std::string name = "human";
    Health health = 100;
    Armor armor = 0;
    Attack attack = 1;
    char signOnField = 'P';
    int army = 7;
    
    int movementPoints = 1;
    int maxMovementPoints = 1;
    bool isStunned = false;
    int attackRange = 1;

    Base* ownerBase = nullptr;

public:
    Unit(std::string n, int hp, int ar, int atk, char s, int color, int mov, int range) : name(n), health(hp), armor(ar), attack(atk), signOnField(s), army(color), movementPoints(mov), maxMovementPoints(mov), attackRange(range) {};
    
    virtual ~Unit() = default;

    virtual std::unique_ptr<Unit> clone() const = 0;
    virtual std::string getType() const = 0;

    std::string getName() const { return name; }
    int getHealth() const { return health.getValue(); }
    int getArmor() const { return armor.getValue(); }
    int getAttack() const { return attack.getValue(); }
    int getColor() const { return army; }
    char getSign() const { return signOnField; }
    int getMovementPoints() const { return movementPoints; }
    int getAttackRange() const { return attackRange; }
    bool getIsStunned() const { return isStunned; }

    void setMovementPoints(int mp) { movementPoints = std::max(0, mp); }
    void resetMovement() { movementPoints = maxMovementPoints; }
    void setStunned(bool value) { isStunned = value; }
    void addMovement(int amount) { movementPoints = std::max(0, movementPoints + amount); }
    void addAttack(int amount) { attack = Attack(attack.getValue() + amount); }
    void addArmor(int amount) { armor = Armor(armor.getValue() + amount); }
    void heal(int amount) { health.increase(amount); }

    void setOwnerBase(Base* base) { ownerBase = base; }
    Base* getOwnerBase() const { return ownerBase; }

    void takeDamage(int amount, int defenseBonus = 0);

    virtual void printInfo() const {
        setColor(army, 0);
        std::cout << "[" << getType() << "] " << name
            << " (HP: " << health.getValue()
            << ", Armor: " << armor.getValue()
            << ", Dmg: " << attack.getValue()
            << ", AttackRange: " << attackRange
            << ", Mov: " << movementPoints << ")";
        setColor(7, 0);
    }
};

class Infantry : public Unit {
public:
    Infantry(std::string n, int hp, int arm, int dmg, char s, int col, int mov = 3, int range = 1)
        : Unit(n, hp, arm, dmg, s, col, mov, range) {
    }
    std::string getType() const override { return "Infantry"; }
};

class Archer : public Unit {
public:
    Archer(std::string n, int hp, int arm, int dmg, char s, int col, int mov = 3, int range = 3)
        : Unit(n, hp, arm, dmg, s, col, mov, range) {
    }
    std::string getType() const override { return "Archer"; }
};

class Cavalry : public Unit {
public:
    Cavalry(std::string n, int hp, int arm, int dmg, char s, int col, int mov = 6, int range = 1)
        : Unit(n, hp, arm, dmg, s, col, mov, range) {
    }
    std::string getType() const override { return "Cavalry"; }
};


class YourSwordsman : public Infantry {
public:
    YourSwordsman() : Infantry("Your Swordsman", 100, 15, 20, 'S', 9, 3, 1) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<YourSwordsman>(*this); }
};

class YourPikeman : public Infantry {
public:
    YourPikeman() : Infantry("Your Pikeman", 100, 20, 15, 'P', 9, 3, 2) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<YourPikeman>(*this); }
};

class YourLightArcher : public Archer {
public:
    YourLightArcher() : Archer("Your LightArcher", 80, 5, 10, 'L', 9, 4, 3) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<YourLightArcher>(*this); }
};

class YourHeavyArcher : public Archer {
public:
    YourHeavyArcher() : Archer("Your HeavyArcher", 80, 10, 20, 'H', 9, 3, 4) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<YourHeavyArcher>(*this); }
};

class YourCataphract : public Cavalry {
public:
    YourCataphract() : Cavalry("Your Cataphract", 100, 50, 30, 'C', 9, 6, 1) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<YourCataphract>(*this); }
};

class YourRider : public Cavalry {
public:
    YourRider() : Cavalry("Your Rider", 80, 5, 10, 'R', 9, 8, 1) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<YourRider>(*this); }
};


class EnemySwordsman : public Infantry {
public:
    EnemySwordsman() : Infantry("Enemy Swordsman", 100, 15, 20, 'S', 12, 3, 1) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<EnemySwordsman>(*this); }
};

class EnemyPikeman : public Infantry {
public:
    EnemyPikeman() : Infantry("Enemy Pikeman", 100, 20, 15, 'P', 12, 3, 2) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<EnemyPikeman>(*this); }
};

class EnemyLightArcher : public Archer {
public:
    EnemyLightArcher() : Archer("Enemy LightArcher", 80, 5, 10, 'L', 12, 4, 3) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<EnemyLightArcher>(*this); }
};

class EnemyHeavyArcher : public Archer {
public:
    EnemyHeavyArcher() : Archer("Enemy HeavyArcher", 80, 10, 20, 'H', 12, 3, 4) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<EnemyHeavyArcher>(*this); }
};

class EnemyCataphract : public Cavalry {
public:
    EnemyCataphract() : Cavalry("Enemy Cataphract", 100, 50, 30, 'C', 12, 6, 1) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<EnemyCataphract>(*this); }
};

class EnemyRider : public Cavalry {
public:
    EnemyRider() : Cavalry("Enemy Rider", 80, 5, 10, 'R', 12, 8, 1) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<EnemyRider>(*this); }
};
