#include "Agent.h"

Agent::Agent(int id_, int startPos) : id(id_), pozycja(startPos) {}

int Agent::getPozycja() const {
    return pozycja;
}

void Agent::ustawPozycje(int nowa) {
    pozycja = nowa;
}

int Agent::getId() const {
    return id;
}
