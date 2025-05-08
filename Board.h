#ifndef BOARD_H
#define BOARD_H
using namespace std;
#include "Player.h"
#include "TownHall.h"
#include "Wall.h"
#include "GoldMine.h"
#include "ElixirCollector.h"
#include "Enemy.h"
#include <vector>
#include <string>

class Board {
private:
    const int width = 147;
    const int height = 33;
    const int margin = 30;

    Player player;
    TownHall townhall;
    vector<Wall> walls;
    vector<GoldMine> goldMines;
    vector<ElixirCollector> elixirCollectors;
    vector<Enemy> enemies;
    vector<string> leftTexts;
    int spawnCounter;
    const int spawnRate;
    bool gameOver;

    void drawBuilding(const Building& building) const;
    bool areBuildingsColliding(const Building& b1, const Building& b2) const;
    bool isPositionOccupied(const Position& pos, const Building* ignore = nullptr) const;
    bool CanBuild(const Building* building, const Building* ignore = nullptr) const;
    void spawnEnemy();
    void updateEnemies();
    void renderTopBorder() const;
    void renderBottomBorder() const;
    void renderMiddle() const;

public:
    Board();
    bool tryMovePlayer(char direction);
    bool placeWall();
    bool placeGoldMine();
    bool placeElixirCollector();
    void collectResources();
    void updateResources();
    void update();
    void render();
};

#endif
