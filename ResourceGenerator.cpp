#include "ResourceGenerator.h"
using namespace std;
ResourceGenerator::ResourceGenerator(int x, int y, int sizeX, int sizeY, int costGold, int costElixir,
                     int health, int maxInstances, const string& icon, int capacity)
    : Building(x, y, sizeX, sizeY, costGold, costElixir, health, maxInstances, icon),
      currentAmount(0), capacity(capacity) {}
