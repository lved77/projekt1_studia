#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void czytajPlik(const char* nazwaPliku) {
    FILE* plik = fopen(nazwaPliku, "r");
    if (!plik) {
        perror("Blad otwarcia pliku");
        return;
    }

    char linia[1024];
    int licznikOutput = 0;

    while (fgets(linia, sizeof(linia), plik)) {
        // usunięcie znaku nowej linii
        linia[strcspn(linia, "\r\n")] = 0;

        // wyświetl odpowiedni znak w zależności od zawartości linii
        if (strlen(linia) == 0) {
            printf("██");
        } else {
            printf("  ");
        }
        
        licznikOutput++;
        // nowa linia co 28 znaków w outpucie
        if (licznikOutput % 28 == 0) {
            printf("\n");
        }
    }

    // dodaj końcową nową linię jeśli ostatni wiersz nie był pełny
    if (licznikOutput % 28 != 0) {
        printf("\n");
    }

    fclose(plik);
}

int main() {
    czytajPlik("graf.txt");
    return 0;
}
