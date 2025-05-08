#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include "Resources.h"

class Player : public Entity {
private:
    Resources resources;
public:
    Player(int x, int y);
    Resources& getResources();
    const Resources& getResources() const;
};

#endif
