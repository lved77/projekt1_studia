#ifndef PUNKT_H
#define PUNKT_H

class Punkt {
    int wierzcholek;
    bool aktywny;

public:
    Punkt(int w) : wierzcholek(w), aktywny(true) {}
    int getWierzcholek() const { return wierzcholek; }
    bool czyAktywny() const { return aktywny; }
    void zjedz() { aktywny = false; }
};

#endif
