#include "Board.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <unistd.h>
#include <termios.h>

using namespace std;

Board::Board() : player(margin + 2, height / 2), 
              townhall(80, height / 2),
              leftTexts(height - 2, string(margin - 1, ' ')),
              spawnCounter(0),
              spawnRate(30),
              gameOver(false) {}

void Board::drawBuilding(const Building& building) const {
    int startX = building.getPosition().x;
    int startY = building.getPosition().y;
    string icon = building.getIcon();

    if (building.Border()) {
        int sizeX = building.getSizeX();
        int sizeY = building.getSizeY();

        cout << "\033[" << startY << ";" << startX << "H┌";
        for (int i = 0; i < sizeX - 2; ++i) cout << "─";
        cout << "┐";

        for (int j = 1; j < sizeY - 1; ++j) {
            cout << "\033[" << startY + j << ";" << startX << "H│";
            for (int i = 1; i < sizeX - 1; ++i) {
                if (i == sizeX/2 && j == sizeY/2) {
                    cout << icon;
                    if (i < sizeX - 2) ++i;
                } else {
                    cout << " ";
                }
            }
            cout << "│";
        }

        cout << "\033[" << startY + sizeY - 1 << ";" << startX << "H└";
        for (int i = 0; i < sizeX - 2; ++i) cout << "─";
        cout << "┘";
    } else {
        cout << "\033[" << startY << ";" << startX << "H" << icon;
    }
}

bool Board::areBuildingsColliding(const Building& b1, const Building& b2) const {
    int x1_min = b1.getPosition().x;
    int y1_min = b1.getPosition().y;
    int x1_max = x1_min + b1.getSizeX();
    int y1_max = y1_min + b1.getSizeY();

    int x2_min = b2.getPosition().x;
    int y2_min = b2.getPosition().y;
    int x2_max = x2_min + b2.getSizeX();
    int y2_max = y2_min + b2.getSizeY();

    bool noOverlap = x1_max <= x2_min || x2_max <= x1_min ||
                     y1_max <= y2_min || y2_max <= y1_min;

    return !noOverlap;
}

bool Board::isPositionOccupied(const Position& pos, const Building* ignore) const {
    for (const auto& wall : walls) {
        if (ignore != &wall &&
            areBuildingsColliding(Building(pos.x, pos.y, 1, 1, 0, 0, 0, 0, "", false), wall)) {
            return true;
        }
    }
    return false;
}

bool Board::CanBuild(const Building* building, const Building* ignore) const {
    for (const auto& wall : walls) {
        if (ignore != &wall && areBuildingsColliding(*building, wall)) return false;
    }
    for (const auto& mine : goldMines) {
        if (ignore != &mine && areBuildingsColliding(*building, mine)) return false;
    }
    for (const auto& collector : elixirCollectors) {
        if (ignore != &collector && areBuildingsColliding(*building, collector)) return false;
    }
    if (areBuildingsColliding(*building, townhall)) return false;

    return true;
}

void Board::spawnEnemy() {
    spawnCounter++;
    if (spawnCounter >= spawnRate) {
        spawnCounter = 0;
        
        static random_device rd;
        static mt19937 gen(rd());
        uniform_int_distribution<> dis(0, 1);
        
        int x, y;
        uniform_int_distribution<> y_dis(1, height - 2);
        y = y_dis(gen);
        
        if (dis(gen)) {
            x = margin + 1;
        } else {
            x = width - 2;
        }
        
        enemies.emplace_back(x, y);
    }
}

void Board::updateEnemies() {
    for (auto& enemy : enemies) {
        if (enemy.update(townhall.getPosition(), walls, goldMines, elixirCollectors, townhall)) {
            gameOver = true;
            return;  // No need to continue; game is over
        }
    }

    walls.erase(remove_if(walls.begin(), walls.end(), 
        [](const Wall& w) { return w.getHealth() <= 0; }), walls.end());
    goldMines.erase(remove_if(goldMines.begin(), goldMines.end(), 
        [](const GoldMine& m) { return m.getHealth() <= 0; }), goldMines.end());
    elixirCollectors.erase(remove_if(elixirCollectors.begin(), elixirCollectors.end(), 
        [](const ElixirCollector& e) { return e.getHealth() <= 0; }), elixirCollectors.end());
}

bool Board::tryMovePlayer(char direction) {
    Position newPos = player.getPosition();
    switch(direction) {
        case 'U': if (newPos.y > 1) newPos.y--; break;
        case 'D': if (newPos.y < height - 2) newPos.y++; break;
        case 'L': if (newPos.x > margin + 2) newPos.x -= 2; break;
        case 'R': if (newPos.x < width - 4) newPos.x += 2; break;
        default: return false;
    }
    if (!isPositionOccupied(newPos)) {
        player.setPosition(newPos.x, newPos.y);
        return true;
    }
    return false;
}

bool Board::placeWall() {
    Position pos = player.getPosition();
    Wall newWall(pos.x, pos.y);

    if (!CanBuild(&newWall)) return false;
    if (walls.size() >= newWall.getMaxInstances()) return false;

    if (player.getResources().gold >= newWall.getCostGold() && player.getResources().elixir >= newWall.getCostElixir()) {
        player.getResources().spendGold(newWall.getCostGold());
        player.getResources().spendElixir(newWall.getCostElixir());
        walls.push_back(newWall);
        return true;
    }

    return false;
}

bool Board::placeGoldMine() {
    Position pos = player.getPosition();
    GoldMine newMine(0, 0);
    int centerX = pos.x - newMine.getSizeX() / 2;
    int centerY = pos.y - newMine.getSizeY() / 2;
    GoldMine mineToPlace(centerX, centerY);

    if (!CanBuild(&mineToPlace)) return false;
    if (goldMines.size() >= newMine.getMaxInstances()) return false;

    if (player.getResources().elixir >= newMine.getCostElixir()) {
        player.getResources().spendElixir(newMine.getCostElixir());
        goldMines.push_back(mineToPlace);
        return true;
    }

    return false;
}

bool Board::placeElixirCollector() {
    Position pos = player.getPosition();
    ElixirCollector newCollector(0, 0);
    int centerX = pos.x - newCollector.getSizeX() / 2;
    int centerY = pos.y - newCollector.getSizeY() / 2;
    ElixirCollector collectorToPlace(centerX, centerY);

    if (!CanBuild(&collectorToPlace)) return false;
    if (elixirCollectors.size() >= newCollector.getMaxInstances()) return false;

    if (player.getResources().gold >= newCollector.getCostGold()) {
        player.getResources().spendGold(newCollector.getCostGold());
        elixirCollectors.push_back(collectorToPlace);
        return true;
    }

    return false;
}

void Board::collectResources() {
    Position pos = player.getPosition();

    for (auto& mine : goldMines) {
        Position bPos = mine.getPosition();
        if (pos.x >= bPos.x && pos.x < bPos.x + mine.getSizeX() &&
            pos.y >= bPos.y && pos.y < bPos.y + mine.getSizeY()) {
            int collected = mine.collect();
            if (collected > 0) {
                player.getResources().gold += collected;
                break;
            }
        }
    }

    for (auto& collector : elixirCollectors) {
        Position bPos = collector.getPosition();
        if (pos.x >= bPos.x && pos.x < bPos.x + collector.getSizeX() &&
            pos.y >= bPos.y && pos.y < bPos.y + collector.getSizeY()) {
            int collected = collector.collect();
            if (collected > 0) {
                player.getResources().elixir += collected;
                break;
            }
        }
    }
}

void Board::updateResources() {
    for (auto& mine : goldMines) mine.update();
    for (auto& collector : elixirCollectors) collector.update();
}

void Board::update() {
    if (gameOver) return;
    spawnEnemy();
    updateEnemies();
    updateResources();
}

void Board::render() {
    system("clear");
    renderTopBorder();
    renderMiddle();
    renderBottomBorder();

    drawBuilding(townhall);
    for (const auto& wall : walls) drawBuilding(wall);
    for (const auto& mine : goldMines) drawBuilding(mine);
    for (const auto& collector : elixirCollectors) drawBuilding(collector);

    for (const auto& enemy : enemies) {
        cout << "\033[" << enemy.getPosition().y << ";" << enemy.getPosition().x << "H";
        cout << enemy.getIcon();
    }

    cout << "\033[" << player.getPosition().y << ";" << player.getPosition().x << "H";
    cout << player.getIcon();

    if (gameOver) {
        string message = "GAME OVER - Town Hall Destroyed!";
        cout << "\033[" << height/2 << ";" << (width - message.length())/2 << "H";
        cout << message;
        cout << "\033[" << height << ";0H";
        exit(0);
    }
}

void Board::renderTopBorder() const {
    cout << "╔";
    for (int x = 1; x < width - 1; x++) {
        cout << (x == margin ? "╦" : "═");
    }
    cout << "╗" << endl;
}

void Board::renderBottomBorder() const {
    cout << "╚";
    for (int x = 1; x < width - 1; x++) {
        cout << (x == margin ? "╩" : "═");
    }
    cout << "╝" << endl;
}

void Board::renderMiddle() const {
    for (int y = 1; y < height - 1; y++) {
        cout << "║";

        if (y == 1) {
            string line = "Gold = " + to_string(player.getResources().gold);
            cout << line << string(margin - 1 - line.length(), ' ');
        } else if (y == 2) {
            string line = "Elixir = " + to_string(player.getResources().elixir);
            cout << line << string(margin - 1 - line.length(), ' ');
        } else if (y == 3) {
            string line = "Walls = " + to_string(walls.size()) + "/200";
            cout << line << string(margin - 1 - line.length(), ' ');
        } else if (y == 4) {
            string line = "Gold Mines = " + to_string(goldMines.size()) + "/3";
            cout << line << string(margin - 1 - line.length(), ' ');
        } else if (y == 5) {
            string line = "Elixir Generators = " + to_string(elixirCollectors.size()) + "/3";
            cout << line << string(margin - 1 - line.length(), ' ');
        } else if (y == 6) {
            string line = "Town Hall HP = " + to_string(townhall.getHealth());
            cout << line << string(margin - 1 - line.length(), ' ');
        } else if (y == 7) {
            string line = "Enemies = " + to_string(enemies.size());
            cout << line << string(margin - 1 - line.length(), ' ');
        } else {
            cout << string(margin - 1, ' ');
        }

        cout << "║";
        cout << string(width - margin - 2, ' ') << "║" << endl;
    }
}
