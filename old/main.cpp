#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include "graf.h"
#include "pacman.h"
#include "duszek.h"
#include "wizualizacja.h"

int main() {
    Graf g;
    g.wczytajZPliku("graf.txt");

    // Dodajmy kilka punktów
    g.dodajPunkt(3);
    g.dodajPunkt(6);
    g.dodajPunkt(8);

    Pacman p1(1, 0);
    Duszek d1(2, 5);

    g.ustawZajety(p1.getPozycja(), true);
    g.ustawZajety(d1.getPozycja(), true);

    for (int i = 0; i < 20; i++) {
        system("cls"); // lub "clear"
        g.wyswietl(); // wizualizacja mapy (na razie prosta)
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        p1.ruch(g);
        d1.ruch(g);
    }

    return 0;
}