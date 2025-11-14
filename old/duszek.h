#ifndef DUSZEK_H
#define DUSZEK_H

#include "Agent.h"
#include <vector>
#include <cstdlib>
#include <ctime>

class Duszek : public Agent {
public:
    Duszek(int id_, int startPos);
    void ruch(Graf& g) override;
};

#endif
