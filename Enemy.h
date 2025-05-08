#ifndef ENEMY_H
#define ENEMY_H
using namespace std;
#include "Npc.h"
#include "Wall.h"
#include "GoldMine.h"
#include "ElixirCollector.h"
#include "TownHall.h"
#include <vector>

class Enemy : public Npc {
private:
    int damage;
    int speedCounter;
    const int speed;
    bool isAttacking;
    Building* targetBuilding;
public:
    Enemy(int x, int y);

bool update(const Position& targetPos, vector<Wall>& walls, vector<GoldMine>& goldMines,
            vector<ElixirCollector>& elixirCollectors, const TownHall& townhall);
    int getDamage() const;
};

#endif


