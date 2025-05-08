#include "Player.h"

Player::Player(int x, int y) : Entity(x, y, "👷"), resources(400, 400) {}

Resources& Player::getResources() { return resources; }
const Resources& Player::getResources() const { return resources; }
