#include <stdio.h>
#include <stdlib.h>
#include "gamelib.h"


int main() {

    int choice;

    do {
        //Stampa del menu
        printf("\n--- Cosestrane ---\n");
        printf("1) Imposta gioco\n");
        printf("2) Gioca\n");
        printf("3) Termina Gioco\n");
        printf("4) Crediti\n");
        printf("Cosa si desidera fare?\n");

        //lettura scelta
        if (scanf("%d", &choice) != 1) {
            printf("Errore, scelta non valida!\n");
            while (getchar() != '\n');
            continue;
        }

        //Scelta da eseguire
        switch(choice) {
            case 1:
                imposta_gioco();
                break;
            
            case 2:
                //verifica_errori();
                //gioca();
                break;

            case 3:
                termina_gioco();
                break;

            case 4:
                crediti();
                break;

            default:
                printf("Errore, scelta non valida!\n");
        }
    } while (choice != 3);

    return 0;
}