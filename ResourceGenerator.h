#ifndef RESOURCEGENERATOR_H
#define RESOURCEGENERATOR_H
using namespace std;
#include "Building.h"

class ResourceGenerator : public Building {
protected:
    int currentAmount;
    const int capacity;
public:
    ResourceGenerator(int x, int y, int sizeX, int sizeY, int costGold, int costElixir,
                     int health, int maxInstances, const string& icon, int capacity);
    virtual void update() = 0;
    virtual int collect() = 0;
};

#endif
