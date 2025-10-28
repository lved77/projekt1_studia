#include "Duszek.h"
#include "graf.h"

Duszek::Duszek(int id_, int startPos)
    : Agent(id_, startPos) {
    std::srand(std::time(nullptr));
}

void Duszek::ruch(Graf& g) {
    auto sasiedzi = g.getSasiedzi(pozycja);
    std::vector<int> mozliwe;

    for (int s : sasiedzi) {
        if (!g.czyZajety(s)) {
            mozliwe.push_back(s);
        }
    }

    if (!mozliwe.empty()) {
        int nowaPozycja = mozliwe[std::rand() % mozliwe.size()];
        g.ustawZajety(pozycja, false);
        ustawPozycje(nowaPozycja);
        g.ustawZajety(nowaPozycja, true);
    }
}
