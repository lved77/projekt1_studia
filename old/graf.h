#ifndef GRAF_H
#define GRAF_H
#include <vector>
#include "Punkt.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>

class Graf {
private:
    std::map<int, std::vector<int>> sasiedzi;  // lista sąsiedztwa
    std::map<int, bool> zajetosc;        // który wierzchołek jest zajęty
    std::vector<Punkt> punkty;
      
public:
    // Wczytuje graf z pliku .txt
    void wczytajZPliku(const std::string& nazwaPliku);

    // Zwraca wektor sąsiadów dla danego wierzchołka
    std::vector<int> getSasiedzi(int v);

    // Ustawia zajętość wierzchołka
    void ustawZajety(int v, bool stan);

    // Sprawdza, czy wierzchołek jest zajęty
    bool czyZajety(int v) const;

    // Zwraca liczbę wierzchołków
    int liczbaWierzcholkow() const;

    void wyswietl() const;
    
    void dodajPunkt(int wierzcholek);

    std::vector<Punkt>& getPunkty();
    
    Punkt* znajdzPunktNaPozycji(int wierzcholek);


    
};

#endif
