#ifndef BUILDING_H
#define BUILDING_H

#include "Position.h"
#include <string>
using namespace std;
class Building {
protected:
    Position pos;
    int sizeX, sizeY;
    int costGold, costElixir;
    int health;
    int maxInstances;
    string icon;
    bool hasBorder;
public:
    Building(int x, int y, int sizeX, int sizeY, int costGold, int costElixir, 
             int health, int maxInstances, const string& icon, bool hasBorder = true);
    
    const Position& getPosition() const;
    const string& getIcon() const;
    int getCostGold() const;
    int getCostElixir() const;
    int getHealth() const;
    int getMaxInstances() const;
    int getSizeX() const;
    int getSizeY() const;
    bool Border() const;
    void setPosition(int x, int y);
    void takeDamage(int damage);
};

#endif
