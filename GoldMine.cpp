#include "GoldMine.h"

GoldMine::GoldMine(int x, int y) : ResourceGenerator(x, y, 7, 3, 0, 100, 100, 3, "🪨", 100) {}

GoldMine& GoldMine::operator=(const GoldMine& other) {
    if (this != &other) {
        pos = other.pos;
        sizeX = other.sizeX;
        sizeY = other.sizeY;
        costGold = other.costGold;
        costElixir = other.costElixir;
        health = other.health;
        maxInstances = other.maxInstances;
        icon = other.icon;
        hasBorder = other.hasBorder;
        currentAmount = other.currentAmount;
    }
    return *this;
}

void GoldMine::update() {
    if (currentAmount < capacity) {
        currentAmount += 5;
        if (currentAmount >= capacity) {
            icon = "🪙";
        }
    }
}

int GoldMine::collect() {
    if (currentAmount >= capacity) {
        int collected = currentAmount;
        currentAmount = 0;
        icon = "🪨";
        return collected;
    }
    return 0;
}
