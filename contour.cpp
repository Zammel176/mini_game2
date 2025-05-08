#include <iostream>
#include <vector>
#include <unistd.h>
#include <termios.h>
#include <algorithm>
#include <thread>
#include <random>

using namespace std;

class Position {
public:
    int x, y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

class Resources {
public:
    int gold, elixir;
    Resources(int g = 400, int e = 400) : gold(g), elixir(e) {}
    bool spendGold(int amount) {
        if (gold >= amount) {
            gold -= amount;
            return true;
        }
        return false;
    }
    bool spendElixir(int amount) {
        if (elixir >= amount) {
            elixir -= amount;
            return true;
        }
        return false;
    }
};

class Entity {
protected:
    Position pos;
    string icon;
public:
    Entity(int x, int y, const string& icon) : pos(x, y), icon(icon) {}
    const Position& getPosition() const { return pos; }
    const string& getIcon() const { return icon; }
    void setPosition(int x, int y) { pos = Position(x, y); }
};

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
             int health, int maxInstances, const string& icon, bool hasBorder = true)
        : pos(x, y), sizeX(sizeX), sizeY(sizeY), costGold(costGold), 
          costElixir(costElixir), health(health), maxInstances(maxInstances), 
          icon(icon), hasBorder(hasBorder) {}
    
    const Position& getPosition() const { return pos; }
    const string& getIcon() const { return icon; }
    int getCostGold() const { return costGold; }
    int getCostElixir() const { return costElixir; }
    int getHealth() const { return health; }
    int getMaxInstances() const { return maxInstances; }
    int getSizeX() const { return sizeX; }
    int getSizeY() const { return sizeY; }
    bool Border() const { return hasBorder; }
    void setPosition(int x, int y) { pos = Position(x, y); }
    void takeDamage(int damage) { health -= damage; }
};

class ResourceGenerator : public Building {
protected:
    int currentAmount;
    const int capacity;
    string emptyIcon;
    string fullIcon;
    
public:
    ResourceGenerator(int x, int y, int sizeX, int sizeY, int costGold, int costElixir,
                     int health, int maxInstances, const string& emptyIcon, 
                     const string& fullIcon, bool hasBorder = true)
        : Building(x, y, sizeX, sizeY, costGold, costElixir, health, maxInstances, emptyIcon, hasBorder),
          currentAmount(0), capacity(100), emptyIcon(emptyIcon), fullIcon(fullIcon) {}
    
    void update() {
        if (currentAmount < capacity) {
            currentAmount += 5;
            if (currentAmount >= capacity) {
                icon = fullIcon;
            }
        }
    }
    
    int collect() {
        if (currentAmount >= capacity) {
            int collected = currentAmount;
            currentAmount = 0;
            icon = emptyIcon;
            return collected;
        }
        return 0;
    }
    
    int getCurrentAmount() const { return currentAmount; }
    int getCapacity() const { return capacity; }
};

class Player : public Entity {
private:
    Resources resources;
public:
    Player(int x, int y) : Entity(x, y, "👷"), resources(400, 400) {}
    Resources& getResources() { return resources; }
    const Resources& getResources() const { return resources; }
};

class Wall : public Building {
public:
    Wall(int x, int y) : Building(x, y, 1, 1, 10, 0, 100, 200, "🧱", false) {}
};

class GoldMine : public ResourceGenerator {
public:
    GoldMine(int x, int y) 
        : ResourceGenerator(x, y, 5, 5, 0, 100, 100, 3, "🪨", "🪙") {}
    
    GoldMine& operator=(const GoldMine& other) {
        if (this != &other) {
            pos = other.pos;
            sizeX = other.sizeX;
            sizeY = other.sizeY;
            costGold = other.costGold;
            costElixir = other.costElixir;
            health = other.health;
            maxInstances = other.maxInstances;
            icon = other.icon;
            hasBorder = other.hasBorder;
            currentAmount = other.currentAmount;
        }
        return *this;
    }
};

class ElixirCollector : public ResourceGenerator {
public:
    ElixirCollector(int x, int y) 
        : ResourceGenerator(x, y, 5, 5, 100, 0, 100, 3, "💧", "🧪") {}
    
    ElixirCollector& operator=(const ElixirCollector& other) {
        if (this != &other) {
            pos = other.pos;
            sizeX = other.sizeX;
            sizeY = other.sizeY;
            costGold = other.costGold;
            costElixir = other.costElixir;
            health = other.health;
            maxInstances = other.maxInstances;
            icon = other.icon;
            hasBorder = other.hasBorder;
            currentAmount = other.currentAmount;
        }
        return *this;
    }
};

class TownHall : public Building {
public:
    TownHall(int x, int y) : Building(x, y, 5, 5, 0, 0, 500, 1, "🏰") {}
};

class Enemy : public Entity {
private:
    int damage;
    int speedCounter;
    const int speed;
    bool isAttacking;
    Building* targetBuilding;
public:
    Enemy(int x, int y) : Entity(x, y, "👹"), damage(10), speedCounter(0), speed(3), 
                         isAttacking(false), targetBuilding(nullptr) {}
    
    void update(const Position& targetPos, vector<Wall>& walls, vector<GoldMine>& goldMines, 
               vector<ElixirCollector>& elixirCollectors, TownHall& townhall) {
        speedCounter++;
        if (speedCounter >= speed) {
            speedCounter = 0;
            
            if (isAttacking && targetBuilding) {
                targetBuilding->takeDamage(damage);
                if (targetBuilding->getHealth() <= 0) {
                    isAttacking = false;
                    targetBuilding = nullptr;
                }
                return;
            }
            
            for (auto& wall : walls) {
                Position wallPos = wall.getPosition();
                if (pos.x == wallPos.x && pos.y == wallPos.y && wall.getHealth() > 0) {
                    isAttacking = true;
                    targetBuilding = &wall;
                    return;
                }
            }
            
            for (auto& mine : goldMines) {
                Position minePos = mine.getPosition();
                if (pos.x >= minePos.x && pos.x < minePos.x + mine.getSizeX() &&
                    pos.y >= minePos.y && pos.y < minePos.y + mine.getSizeY() && 
                    mine.getHealth() > 0) {
                    isAttacking = true;
                    targetBuilding = &mine;
                    return;
                }
            }
            
            for (auto& collector : elixirCollectors) {
                Position colPos = collector.getPosition();
                if (pos.x >= colPos.x && pos.x < colPos.x + collector.getSizeX() &&
                    pos.y >= colPos.y && pos.y < colPos.y + collector.getSizeY() && 
                    collector.getHealth() > 0) {
                    isAttacking = true;
                    targetBuilding = &collector;
                    return;
                }
            }
            
            Position thPos = townhall.getPosition();
            if (pos.x >= thPos.x && pos.x < thPos.x + townhall.getSizeX() &&
                pos.y >= thPos.y && pos.y < thPos.y + townhall.getSizeY()) {
                isAttacking = true;
                targetBuilding = &townhall;
                return;
            }
            
            if (pos.x < targetPos.x) pos.x++;
            else if (pos.x > targetPos.x) pos.x--;
            
            if (pos.y < targetPos.y) pos.y++;
            else if (pos.y > targetPos.y) pos.y--;
        }
    }
    
    int getDamage() const { return damage; }
};

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

    void drawBuilding(const Building& building) const {
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

    bool areBuildingsColliding(const Building& b1, const Building& b2) const {
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

    bool isPositionOccupied(const Position& pos, const Building* ignore = nullptr) const {
        for (const auto& wall : walls) {
            if (ignore != &wall &&
                areBuildingsColliding(Building(pos.x, pos.y, 1, 1, 0, 0, 0, 0, "", false), wall)) {
                return true;
            }
        }
        return false;
    }

    bool CanBuild(const Building* building, const Building* ignore = nullptr) const {
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

    void spawnEnemy() {
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

    void updateEnemies() {
        for (auto& enemy : enemies) {
            enemy.update(townhall.getPosition(), walls, goldMines, elixirCollectors, townhall);
            
            if (townhall.getHealth() <= 0) {
                gameOver = true;
            }
        }
        
        walls.erase(remove_if(walls.begin(), walls.end(), 
            [](const Wall& w) { return w.getHealth() <= 0; }), walls.end());
        goldMines.erase(remove_if(goldMines.begin(), goldMines.end(), 
            [](const GoldMine& m) { return m.getHealth() <= 0; }), goldMines.end());
        elixirCollectors.erase(remove_if(elixirCollectors.begin(), elixirCollectors.end(), 
            [](const ElixirCollector& e) { return e.getHealth() <= 0; }), elixirCollectors.end());
    }

public:
    Board() : player(margin + 2, height / 2), 
              townhall(80, height / 2),
              leftTexts(height - 2, string(margin - 1, ' ')),
              spawnCounter(0),
              spawnRate(30),
              gameOver(false) {}

    bool tryMovePlayer(char direction) {
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

    bool placeWall() {
        Position pos = player.getPosition();
        Wall newWall(pos.x, pos.y);

        if (!CanBuild(&newWall)) {
            return false;
        }

        if (walls.size() >= newWall.getMaxInstances()) {
            return false;
        }

        if (player.getResources().gold >= newWall.getCostGold() && player.getResources().elixir >= newWall.getCostElixir()) {
            player.getResources().spendGold(newWall.getCostGold());
            player.getResources().spendElixir(newWall.getCostElixir());
            walls.push_back(newWall);
            return true;
        }

        return false;
    }

    bool placeGoldMine() {
        Position pos = player.getPosition();
        GoldMine newMine(0, 0);
        int centerX = pos.x - newMine.getSizeX() / 2;
        int centerY = pos.y - newMine.getSizeY() / 2;
        GoldMine mineToPlace(centerX, centerY);

        if (!CanBuild(&mineToPlace)) {
            return false;
        }

        if (goldMines.size() >= newMine.getMaxInstances()) {
            return false;
        }

        if (player.getResources().elixir >= newMine.getCostElixir()) {
            player.getResources().spendElixir(newMine.getCostElixir());
            goldMines.push_back(mineToPlace);
            return true;
        }

        return false;
    }

    bool placeElixirCollector() {
        Position pos = player.getPosition();
        ElixirCollector newCollector(0, 0);
        int centerX = pos.x - newCollector.getSizeX() / 2;
        int centerY = pos.y - newCollector.getSizeY() / 2;
        ElixirCollector collectorToPlace(centerX, centerY);

        if (!CanBuild(&collectorToPlace)) {
            return false;
        }

        if (elixirCollectors.size() >= newCollector.getMaxInstances()) {
            return false;
        }

        if (player.getResources().gold >= newCollector.getCostGold()) {
            player.getResources().spendGold(newCollector.getCostGold());
            elixirCollectors.push_back(collectorToPlace);
            return true;
        }

        return false;
    }

    void collectResources() {
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

    void updateResources() {
        for (auto& mine : goldMines) {
            mine.update();
        }
        for (auto& collector : elixirCollectors) {
            collector.update();
        }
    }

    void update() {
        if (gameOver) return;
        
        spawnEnemy();
        updateEnemies();
        updateResources();
    }

    void render() {
        system("clear");
        renderTopBorder();
        renderMiddle();
        renderBottomBorder();
        
        drawBuilding(townhall);
        for (const auto& wall : walls) {
            drawBuilding(wall);
        }
        for (const auto& mine : goldMines) {
            drawBuilding(mine);
        }
        for (const auto& collector : elixirCollectors) {
            drawBuilding(collector);
        }
        
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

private:
    void renderTopBorder() const {
        cout << "╔";
        for (int x = 1; x < width - 1; x++) {
            cout << (x == margin ? "╦" : "═");
        }
        cout << "╗" << endl;
    }

    void renderBottomBorder() const {
        cout << "╚";
        for (int x = 1; x < width - 1; x++) {
            cout << (x == margin ? "╩" : "═");
        }
        cout << "╝" << endl;
    }

    void renderMiddle() const {
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
};

class InputManager {
private:
    termios originalTerminalSettings;
public:
    InputManager() {
        tcgetattr(STDIN_FILENO, &originalTerminalSettings);
        termios newSettings = originalTerminalSettings;
        newSettings.c_lflag &= ~(ICANON | ECHO);
        newSettings.c_cc[VMIN] = 1;
        newSettings.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
    }

    ~InputManager() {
        tcsetattr(STDIN_FILENO, TCSANOW, &originalTerminalSettings);
    }

    char getInput() const {
        char ch = getchar();
        if (ch == 27) {
            getchar();
            ch = getchar();
            switch(ch) {
                case 'A': return 'U';
                case 'B': return 'D';
                case 'C': return 'R';
                case 'D': return 'L';
            }
        }
        return toupper(ch);
    }
};

int main() {
    cout << "\033[?25l";
    Board board;
    InputManager inputManager;

    while (true) {
        board.render();
        char input = inputManager.getInput();

        switch(input) {
            case 'U': case 'D': case 'L': case 'R':
                board.tryMovePlayer(input);
                break;
            case 'W':
                board.placeWall();
                break;
            case 'M':
                board.placeGoldMine();
                break;
            case 'E':
                board.placeElixirCollector();
                break;
            case 'C':
                board.collectResources();
                break;
            case 'Q':
                cout << "\033[?25h";
                return 0;
        }

        board.update();
        usleep(100000);
    }
    cout << "\033[?25h";
    return 0;
}
