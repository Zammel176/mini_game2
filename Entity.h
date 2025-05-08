#ifndef ENTITY_H
#define ENTITY_H
using namespace std;
#include "Position.h"
#include <string>

class Entity {
protected:
    Position pos;
    string icon;
public:
    Entity(int x, int y, const string& icon);
    const Position& getPosition() const;
    const string& getIcon() const;
    void setPosition(int x, int y);
};

#endif
