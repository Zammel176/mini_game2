#include "Resources.h"

Resources::Resources(int g, int e) : gold(g), elixir(e) {}

bool Resources::spendGold(int amount) {
    if (gold >= amount) {
        gold -= amount;
        return true;
    }
    return false;
}

bool Resources::spendElixir(int amount) {
    if (elixir >= amount) {
        elixir -= amount;
        return true;
    }
    return false;
}
