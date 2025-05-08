#include "Entity.h"
using namespace std;
Entity::Entity(int x, int y, const string& icon) : pos(x, y), icon(icon) {}

const Position& Entity::getPosition() const {
    return pos;
}
const string& Entity::getIcon() const { return icon; }
void Entity::setPosition(int x, int y) {
    pos = Position(x, y);
}
