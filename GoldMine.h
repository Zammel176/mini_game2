#ifndef GOLDMINE_H
#define GOLDMINE_H

#include "ResourceGenerator.h"

class GoldMine : public ResourceGenerator {
public:
    GoldMine(int x, int y);
    GoldMine& operator=(const GoldMine& other);
    void update() override;
    int collect() override;
};

#endif
