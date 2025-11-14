#include "graf.h"

void Graf::wczytajZPliku(const std::string& nazwaPliku) {
    std::ifstream plik(nazwaPliku);
    if (!plik.is_open()) {
        std::cerr << "Blad: nie mozna otworzyc pliku " << nazwaPliku << std::endl;
        return;
    }

    std::string linia;
    int numerWierzcholka = 0;

    while (std::getline(plik, linia)) {
        std::stringstream ss(linia);
        int sasiad;
        std::vector<int> listaSasiadow;

        // odczytujemy numery sąsiadów z linii
        while (ss >> sasiad) {
            listaSasiadow.push_back(sasiad);
        }

        // zapisujemy w mapie
        sasiedzi[numerWierzcholka] = listaSasiadow;
        zajetosc[numerWierzcholka] = false; // domyślnie wierzchołek wolny

        numerWierzcholka++;
    }

    plik.close();
}

std::vector<int> Graf::getSasiedzi(int v) {
    if (sasiedzi.find(v) != sasiedzi.end()) {
        return sasiedzi[v];
    } else {
        return {};
    }
}

void Graf::ustawZajety(int v, bool stan) {
    if (zajetosc.find(v) != zajetosc.end()) {
        zajetosc[v] = stan;
    }
}

bool Graf::czyZajety(int v) const {
    auto it = zajetosc.find(v);
    if (it != zajetosc.end()) {
        return it->second;
    }
    return false;
}

int Graf::liczbaWierzcholkow() const {
    return static_cast<int>(sasiedzi.size());
}

void Graf::dodajPunkt(int wierzcholek) {
    punkty.emplace_back(wierzcholek);
}

std::vector<Punkt>& Graf::getPunkty() {
    return punkty;
}

Punkt* Graf::znajdzPunktNaPozycji(int wierzcholek) {
    for (auto& p : punkty) {
        if (p.getWierzcholek() == wierzcholek && p.czyAktywny())
            return &p;
    }
    return nullptr;
}
