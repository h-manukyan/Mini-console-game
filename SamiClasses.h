#ifndef SAMI_CLASSES_H
#define SAMI_CLASSES_H

#include <iostream>
#include <map>
#include <string>
#include <algorithm>

using namespace std;

class Entity {
protected:
    map<string, int> stats = {
        {"Health", 0},
        {"Damage", 0},
        {"CritRate", 15},
        {"MinRollToHit", 1}
    };
public:
    Entity(int health, int damage, int critRate = 15, int minRollToHit = 1) {
        stats["Health"] = health;
        stats["Damage"] = damage;
        stats["CritRate"] = critRate;
        stats["MinRollToHit"] = minRollToHit;
    }

    virtual ~Entity() = default;

    void SetStat(const string& statname, int statvalue) {
        stats[statname] = statvalue;
    }

    int GetStat(const string& statname) const {
        auto it = stats.find(statname);
        return (it != stats.end()) ? it->second : 0;
    }

    bool IsAlive() const {
        return GetStat("Health") > 0;
    }

    void TakeDamage(int damage) {
        int currentHP = GetStat("Health");
        SetStat("Health", max(0, currentHP - damage));
    }
};

class Enemy : public Entity {
public:
    Enemy(int health, int damage, int critRate = 15, int minRollToHit = 3)
        : Entity(health, damage, critRate, minRollToHit) {}
};

class Sami : public Entity {
private:
    int gold = 0;
    int maxHealth = 100;
public:
    Sami(int health, int energy, int damage, int critRate = 15, int minRollToHit = 1)
        : Entity(health, damage, critRate, minRollToHit), maxHealth(health) {
        stats["Energy"] = energy;
    }

    int GetGold() const { return gold; }
    void AddGold(int amount) { gold += amount; }
    bool SpendGold(int amount) {
        if (gold >= amount) {
            gold -= amount;
            return true;
        }
        return false;
    }

    int GetMaxHP() const { return maxHealth; }
    void SetMaxHP(int hp) {
        maxHealth = hp;
        SetStat("Health", hp);
    }

    void HealFull() {
        SetStat("Health", maxHealth);
    }
};

#endif // SAMI_CLASSES_H