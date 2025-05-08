#ifndef RESOURCES_H
#define RESOURCES_H

class Resources {
public:
    int gold, elixir;
    Resources(int g = 400, int e = 400);
    bool spendGold(int amount);
    bool spendElixir(int amount);
};

#endif
