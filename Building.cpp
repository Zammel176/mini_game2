#include "Building.h"
using namespace std;
Building::Building(int x, int y, int sizeX, int sizeY, int costGold, int costElixir, 
         int health, int maxInstances, const string& icon, bool hasBorder)
    : pos(x, y), sizeX(sizeX), sizeY(sizeY), costGold(costGold), 
      costElixir(costElixir), health(health), maxInstances(maxInstances), 
      icon(icon), hasBorder(hasBorder) {}

const Position& Building::getPosition() const { return pos; }
const string& Building::getIcon() const { return icon; }
int Building::getCostGold() const { return costGold; }
int Building::getCostElixir() const { return costElixir; }
int Building::getHealth() const { return health; }
int Building::getMaxInstances() const { return maxInstances; }
int Building::getSizeX() const { return sizeX; }
int Building::getSizeY() const { return sizeY; }
bool Building::Border() const { return hasBorder; }
void Building::setPosition(int x, int y) { pos = Position(x, y); }
void Building::takeDamage(int damage) { health -= damage; }
