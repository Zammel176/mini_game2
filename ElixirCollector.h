#ifndef ELIXIRCOLLECTOR_H
#define ELIXIRCOLLECTOR_H

#include "ResourceGenerator.h"

class ElixirCollector : public ResourceGenerator {
public:
    ElixirCollector(int x, int y);
    ElixirCollector& operator=(const ElixirCollector& other);
    void update() override;
    int collect() override;
};

#endif
