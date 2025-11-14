#ifndef AGENT_H
#define AGENT_H

#include "graf.h"

class Agent {
protected:
    int id;
    int pozycja;

public:
    Agent(int id_, int startPos);
    virtual ~Agent() = default;

    int getPozycja() const;
    void ustawPozycje(int nowa);
    int getId() const;

    // Metoda wirtualna — różna dla Pacmana i Duszków
    virtual void ruch(Graf& g) = 0;
};

#endif
