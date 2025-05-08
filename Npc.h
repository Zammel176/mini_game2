#ifndef NPC_H
#define NPC_H

#include "Entity.h"

class Npc : public Entity {
public:
    Npc(int x, int y, const std::string& icon);
};

#endif
