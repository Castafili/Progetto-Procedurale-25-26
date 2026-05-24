#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "gamelib.h"

#define AC_RED "\x1b[31m"
#define AC_YELLOW "\x1b[33m"
#define AC_NORMAL "\x1b[m"

// Vaariabili globaali

static Giocatore *giocatori[4] = {NULL, NULL, NULL, NULL};
static int n_giocatori = 0;
static Zona_mondoreale *prima_zona_mondoreale = NULL;
static Zona_soprasotto *prima_zona_soprasotto = NULL;
static int mappa_chiusa = 0; // Non chiusa (1 -> chiusa)
static int gioco_impostat = 0; // Come sopr
static char vincitori_precedenti[3][50] = {"", "", ""}; 
static int n_vincitori = 0;

// Funzioni extra

// Lancio del dado
static int lancia_dado(int facce) {
    return (rand() % facce) + 1;
}

// Check del demotorzone
static int conta_demotorzone() {
    int count = 0;
    Zona_soprasotto *temp = prima_zona_soprasotto;
    while (temp != NULL) {
        if (temp->nemico == demotorzone) {
            count++;
        }
        temp = temp->avanti;
    }
    return count;
}



// Deallaaocazione mappa Mondo Reaale
static void dealloca_mondoreale() {
    Zona_mondoreale *current = prima_zona_mondoreale;
    while (current != NULL) {
        Zona_mondoreale *next = current->avanti;
        free(current);
        current = next;
    }
    prima_zona_mondoreale = NULL;
}

// Deallaaocazione mappa Soprasotto
static void dealloca_soprasotto() {
    Zona_soprasotto *current = prima_zona_soprasotto;
    while (current != NULL) {
        Zona_soprasotto *next = current->avanti;
        free(current);
        current = next;
    }
    prima_zona_soprasotto = NULL;
}

// Deallocazione giocatori
static void dealloca_gioctori() {
    for (int i=0; i < 4; i++) {
        if (giocatori[i] != NULL) {
            free(giocatori[i]);
            giocatori[i] = NULL;
        }
    }
    n_giocatori = 0;
}



// Generazione Tipo_zona
static Tipo_zona genera_tipo_zona_randomico() {
    return (Tipo_zona)(rand() % 10);
}

// Generazione nemico randomico Mondo Reale
static Tipo_nemico genera_nemico_mondoreale() {
    int r = rand() % 100;
    if (r < 40) return nessun_nemico;
    else if (r < 70) return democane;
    else return billi;
}

// Generazione nemico randomico Soprasotto
static Tipo_nemico genera_nemico_soprasotto(int demotorzone_forzato) {
    if (demotorzone_forzato) {
        return demotorzone;
    }

    int r = rand() % 100;
    if (r < 40) return nessun_nemico;
    else return democane;
} 

// Generazione oggetto randomico (Solo Mondo Reale)
static Tipo_oggetto genera_oggetto_randomico() {
    int r = rand() % 100;
    if (r < 50) return nessun_oggetto;
    else {
        return (Tipo_oggetto)(1 + (rand() % 4));
    }
}



// Funzioni Creazione Mappa

static void genera_mappa() {
    printf("\n=== Generazione Mappa ===\n");

    //Deallocazione mappa precedente
    dealloca_mondoreale();
    dealloca_soprasotto();
    mappa_chiusa = 0;

    // Check demotorzone
    int posizione_demotorzone = rand() % 15;

    // Creazione 15 zone
    for (int i = 0; i < 15; i++) {

        // Crea la parte di Mondo Reale
        Zona_mondoreale *nuova_mr = (Zona_mondoreale*)malloc(sizeof(Zona_mondoreale));
        if (nuova_mr == NULL) {
            printf("Errore alloczione memoria\n", AC_RED, AC_NORMAL);
            return;
        }
        nuova_mr -> tipo = genera_tipo_zona_randomico();
        nuova_mr -> nemico = genera_nemico_mondoreale();
        nuova_mr -> oggetto = genera_oggetto_randomico();
        nuova_mr -> avanti = NULL;
        nuova_mr -> indietro = NULL;
        nuova_mr -> link_soprasotto = NULL;

        // Crea Soprasotto corrispondente
        Zona_soprasotto *nuova_ss = (Zona_soprasotto*)malloc(sizeof(Zona_soprasotto));
        if (nuova_ss == NULL) {
            printf("Errore alloczione memoria\n", AC_RED, AC_NORMAL);
            free(nuova_mr);
            return;
        }
        nuova_ss -> tipo = nuova_mr; // Per esseere la stessa zona del Mondo Rale
        nuova_ss -> nemico = genera_nemico_soprasotto (i == posizione_demotorzone);
        nuova_ss -> avanti = NULL;
        nuova_ss -> indietro = NULL;
        nuova_ss -> link_mondoreale = nuova_mr;

        // Collegamnto mondi
        nuova_mr -> link_soprasotto = nuova_ss;

        
        // Inserimento lista Mondo Reale
        if (prima_zona_mondoreale == NULL) {
            prima_zona_mondoreale = nuova_mr;
        } else {
            Zona_mondoreale *temp = prima_zona_mondoreale;
            while (temp -> avanti != NULL) {
                temp = temp -> avanti;
            }
            temp -> avanti = nuova_mr;
            nuova_mr -> indietro = temp;
        }

        // Inserimento lista Soprasotto
        if (prima_zona_soprasotto == NULL) {
            prima_zona_soprasotto = nuova_ss;
        } else {
            Zona_soprasotto *temp = prima_zona_soprasotto;
            while (temp -> avanti != NULL) {
                temp = temp -> avanti;
            }
            temp -> avanti = nuova_ss;
            nuova_ss -> indietro = temp;
        }
    }

    printf("Mappa generata con succeso! 15 zone create.\n");
}


static void inserisci_zona() {
    int posizione;
    printf("\nIn che posizione vuoi insrire la nuova zona? (1-%d): ", conta_zone_mondoreale() + 1);
    if (scanf("%d", &posizione) !=1) {
        printf("Input non valido!\n", AC_RED, AC_NORMAL);
        while (getchar() != '\n');
        return;
    }

    if (posizione < 1) {
        printf("Posizione non valida!\n", AC_RED, AC_NORMAL);
        return;
    }

    // Chiedi valori al giocatore
    int tipo, nemico_mr, nemico_ss, oggetto;

    printf("Tipo zona (0-9): ");
    printf("\n0=Bosco, 1=Scuola, 2=Laboratorio, 3=Caverna, 4=Strada,\n"); 
    printf("5=Giardino, 6=Supermercato, 7=Centrale Elettrica,\n"); 
    printf("8=Deposito Abbandonato, 9=Stazion Polizia\n");

    if (scanf("%d", &tipo) != 1 || tipo < 0 || tipo > 9) {
        printf("Tipo non valido!\n", AC_RED, AC_NORMAL);
        while (getchar() != '\n');
        return;
    } 

    printf("Nemico Mondo Reale (0=Neessuno, 1=Billi, 2=Democane): ");
    if (scanf("%d", &nemico_mr) != 1 || nemico_mr < 0 || nemico_mr > 2) {
        printf("Nemico non valido!\n", AC_RED, AC_NORMAL);
            while (getchar() != '\n');
            return;
    }

    printf("Nemico Soprasotto (0=Nessuno, 2=Democane, 3=Demotorzone): ");
    if (scanf("%d", &nemico_ss) != 1 || nemico_ss < 0 || nemico_ss > 3 || nemico_ss == 1) {
        printf("Nemico non valido!\n", AC_RED, AC_NORMAL);
            while (getchar() != '\n');
            return;
    }

    printf("Oggetto (0=Nessuno, 1=Bicicletta, 2=Maglietta, 3=Bussola, 4=Schitarrata): ");
    if (scanf("%d", &oggetto) != 1 || oggetto < 0 || oggetto > 4) {
        printf("Oggetto non valido!\n", AC_RED, AC_NORMAL);
            while (getchar() != '\n');
            return;
    }

    // Crea nuove stanze
    Zona_mondoreale *nuova_mr = (Zona_mondoreale*)maalloc(sizeof(Zona_mondoreale));
    Zona_soprasotto *nuova_ss = (Zona_soprasotto*)maalloc(sizeof(Zona_soprasotto));

    if (nuova_mr == NULL || nuova_ss == NULL) {
        printf("Errore allocazione memoria!\n", AC_RED, AC_NORMAL);
        free(nuova_mr);
        free(nuova_ss);
        return;
    }

    nuova_mr -> tipo = (Tipo_zona)tipo;
    nuova_mr -> nemico = (Tipo_nemico)nemico_mr;
    nuova_mr -> oggetto = (Tipo_oggetto)oggetto;
    nuova_mr -> avanti = NULL;
    nuova_mr -> indietro = NULL;

    nuova_ss -> tipo = (Tipo_zona)tipo;
    nuova_ss -> nemico = (Tipo_nemico)nemico_ss;
    nuova_ss -> avanti = NULL;
    nuova_ss -> indietro = NULL;

    nuova_mr -> link_soprasotto = nuova_ss;
    nuova_ss -> link_mondoreale = nuova_mr;

    //Inserimnto nella lista Mondo Reale
    if (posizione == 1)  {
        nuova_mr -> avanti = prima_zona_mondoreale;
        if (prima_zona_mondoreale != NULL) {
            prima_zona_mondoreale -> indietro = nuova_mr;
        }
        prima_zona_mondoreale = nuova_mr;
    } else {
        Zona_mondoreale *temp = prima_zona_mondoreale;
        for (int i = 1; i < posizione - 1 && temp != NULL; i++) {
            temp = temp -> avanti;
        }

        if (temp == NULL) {
            printf("Posizione troppo grande, inserisco alla fine.\n", AC_YELLOW, AC_NORMAL);
            temp = prima_zona_mondoreale;
            while (temp -> avanti != NULL) {
                temp = temp -> avanti;
            }

            temp -> avanti = nuova_mr;
            nuova_mr -> indietro = temp;
            
        } else {
            nuova_mr -> avanti = temp -> avanti;
            nuova_mr -> indietro = temp;
            if (temp -> avanti != NULL) {
                temp -> avanti -> indietro = nuova_mr;
            }
            temp -> avanti = nuova_mr;
        }
    }

    //Inserimnto nella lista Soprasotto
    if (posizione == 1) {
        nuova_ss -> avanti = prima_zona_soprasotto;
        if (prima_zona_soprasotto != NULL) {
            prima_zona_soprasotto -> indietro = nuova_ss;
        }
        prima_zona_soprasotto = nuova_ss;
    } else {
        Zona_soprasotto *temp = prima_zona_soprasotto;
        for (int i = 1; i < posizione -1 && temp != NULL; i++) {
            temp = temp -> avanti;
        }

        if (temp == NULL) {
            temp = prima_zona_soprasotto;
            while (temp -> avanti != NULL) {
                temp = temp -> avanti;
            }

            temp -> avanti = nuova_ss;
            nuova_ss -> indietro = temp;

        } else {
            nuova_ss -> avanti = temp -> avanti;
            nuova_ss -> indietro = temp;
            if (temp -> avanti != NULL) {
                temp -> avanti -> indietro = nuova_ss;
            }
            temp -> avanti = nuova_ss;
        }
    }

    printf("Zona inserita con successo!\n");
    mappa_chiusa = 0;

}


static void cancella_zona() {

}