#include "Pacman.h"
#include "Graf.h"
#include <iostream>

Pacman::Pacman(int id_, int startPos)
    : Agent(id_, startPos) {
    std::srand(std::time(nullptr));
}

void Pacman::ruch(Graf& g) {
    auto sasiedzi = g.getSasiedzi(pozycja);
    std::vector<int> mozliwe;

    // wybieramy tylko wolne wierzchołki
    for (int s : sasiedzi) {
        if (!g.czyZajety(s)) {
            mozliwe.push_back(s);
        }
    }

    // jeśli są wolne miejsca — wylosuj ruch
    if (!mozliwe.empty()) {
        int nowaPozycja = mozliwe[std::rand() % mozliwe.size()];
        g.ustawZajety(pozycja, false);
        ustawPozycje(nowaPozycja);
        g.ustawZajety(nowaPozycja, true);
    }

    Punkt* p = g.znajdzPunktNaPozycji(pozycja);
    if (p) {
        p->zjedz();
        std::cout << "Pac-Man zjadl punkt w wierzcholku " << pozycja << "!\n";
    }
}
