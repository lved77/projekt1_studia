#ifndef PACMAN_H
#define PACMAN_H

#include "Agent.h"
#include <vector>
#include <cstdlib>
#include <ctime>

class Pacman : public Agent {
public:
    Pacman(int id_, int startPos);
    void ruch(Graf& g) override;
};

#endif
