#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "gamelib.h"

// ========= FIXES ========


// Prototipi funzioni statiche
static const char* nome_tipo_zona(Tipo_zona tipo);
static const char* nome_tipo_nemico(Tipo_nemico nemico);
static const char* nome_tipo_oggetto(Tipo_oggetto oggetto);
static int lancia_dado(int facce);
static int conta_zone_mondoreale();
static int utilizza_oggetto_giocatore(int indice, int *bonus_attacco_temp);

// Variabili globali

static Giocatore *giocatori[4] = {NULL, NULL, NULL, NULL};
static int n_giocatori = 0;
static Zona_mondoreale *prima_zona_mondoreale = NULL;
static Zona_soprasotto *prima_zona_soprasotto = NULL;
static int mappa_chiusa = 0; // Non chiusa (1 -> chiusa)
static int gioco_impostato = 0; // Come sopra
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

// Check zone nel Mondo Reale
static int conta_zone_mondoreale() {
    int count = 0;
    Zona_mondoreale *temp = prima_zona_mondoreale;
    while (temp != NULL) {
        count++;
        temp = temp->avanti;
    }
    return count;
}

// Ottiene il nome dell'enum Tipo_zona
static const char* nome_tipo_zona(Tipo_zona tipo) {
    switch (tipo) {
        case bosco: return "Bosco";
        case scuola: return "Scuola";
        case laboratorio: return "Laboratorio";
        case caverna: return "Caverna";
        case strada: return "Strada";
        case giardino: return "Giardino";
        case supermercato: return "Supermercato";
        case centrale_elettrica: return "Centrale Elettrica";
        case deposito_abbandonato: return "Deposito Abbandonato";
        case stazione_polizia: return "Stazione di Polizia";
        default: return "Sconosciuto";
    }
}

// Ottiene il nome dell'enum Tipo_nemico
static const char* nome_tipo_nemico(Tipo_nemico nemico) {
    switch (nemico) {
        case nessun_nemico: return "Nessuno";
        case billi: return "Billi";
        case democane: return "Democane";
        case demotorzone: return "Demotorzone";
        default: return "Sconosciuto";
    }
}

// Ottiene il nome dell'enum Tipo_oggetto
static const char* nome_tipo_oggetto(Tipo_oggetto oggetto) {
    switch (oggetto) {
        case nessun_oggetto: return "Nessuno";
        case bicicletta: return "Bicicletta";
        case maglietta_fuocoinferno: return "Maglietta Fuocoinferno";
        case bussola: return "Bussola";
        case schitarrata_metallica: return "Schitarrata Metallica";
        default: return "Sconosciuto";
    }
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
            printf("Errore alloczione memoria\n");
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
            printf("Errore alloczione memoria\n");
            free(nuova_mr);
            return;
        }
        nuova_ss -> tipo = nuova_mr -> tipo; // Per esseere la stessa zona del Mondo Rale
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
    printf("\nIn che posizione vuoi inserire la nuova zona? (1-%d): ", conta_zone_mondoreale() + 1);
    if (scanf("%d", &posizione) !=1) {
        printf("Input non valido!\n");
        while (getchar() != '\n');
        return;
    }

    if (posizione < 1) {
        printf("Posizione non valida!\n");
        return;
    }

    // Chiedi valori al giocatore
    int tipo, nemico_mr, nemico_ss, oggetto;

    printf("Tipo zona (0-9): ");
    printf("\n0=Bosco, 1=Scuola, 2=Laboratorio, 3=Caverna, 4=Strada,\n"); 
    printf("5=Giardino, 6=Supermercato, 7=Centrale Elettrica,\n"); 
    printf("8=Deposito Abbandonato, 9=Stazione Polizia\n");

    if (scanf("%d", &tipo) != 1 || tipo < 0 || tipo > 9) {
        printf("Tipo non valido!\n");
        while (getchar() != '\n');
        return;
    } 

    printf("Nemico Mondo Reale (0=Nessuno, 1=Billi, 2=Democane): ");
    if (scanf("%d", &nemico_mr) != 1 || nemico_mr < 0 || nemico_mr > 2) {
        printf("Nemico non valido!\n");
            while (getchar() != '\n');
            return;
    }

    printf("Nemico Soprasotto (0=Nessuno, 2=Democane, 3=Demotorzone): ");
    if (scanf("%d", &nemico_ss) != 1 || nemico_ss < 0 || nemico_ss > 3 || nemico_ss == 1) {
        printf("Nemico non valido!\n");
            while (getchar() != '\n');
            return;
    }

    printf("Oggetto (0=Nessuno, 1=Bicicletta, 2=Maglietta, 3=Bussola, 4=Schitarrata): ");
    if (scanf("%d", &oggetto) != 1 || oggetto < 0 || oggetto > 4) {
        printf("Oggetto non valido!\n");
            while (getchar() != '\n');
            return;
    }

    // Crea nuove stanze
    Zona_mondoreale *nuova_mr = (Zona_mondoreale*) malloc(sizeof(Zona_mondoreale));
    Zona_soprasotto *nuova_ss = (Zona_soprasotto*) malloc(sizeof(Zona_soprasotto));

    if (nuova_mr == NULL || nuova_ss == NULL) {
        printf("Errore allocazione memoria!\n");
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
            printf("Posizione troppo grande, inserisco alla fine.\n");
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
    int posizione; 
    int n_zone = conta_zone_mondoreale();

    if (n_zone == 0) {
        printf("Non ci sono zone da cancellare!\n");
        return;
    }

    printf("\nQuale zona vuoi cancellare? (1-%d): ", n_zone);
    if (scanf("%d", &posizione) != 1 || posizione < 1 || posizione > n_zone) {
        printf("Posizion non valida!\n");
        while (getchar() != '\n');
        return;
    }

    // Trova zona da rimuovere nel Mondo Reale
    Zona_mondoreale *temp_mr = prima_zona_mondoreale;
    for (int i = 1; i < posizione && temp_mr != NULL; i++) {
        temp_mr = temp_mr -> avanti;
    }

    if (temp_mr == NULL) {
        printf("Zona non trovta!");
        return;
    }

    // Trova la zona corrispondente nel Soprasotto
    Zona_soprasotto *temp_ss = temp_mr -> link_soprasotto;

    // Rimozzione da Mondo Reale
    if (temp_mr -> indietro != NULL) {
        temp_mr -> indietro -> avanti = temp_mr -> avanti;
    } else {
        prima_zona_mondoreale = temp_mr -> avanti;
    }

    if (temp_mr -> avanti != NULL) {
        temp_mr -> avanti -> indietro = temp_mr -> indietro;
    }

    // Rimozzione da Soprasotto
    if (temp_ss -> indietro != NULL) {
        temp_ss -> indietro -> avanti = temp_ss -> avanti;
    } else {
        prima_zona_soprasotto = temp_ss -> avanti;
    }

    if (temp_ss -> avanti != NULL) {
        temp_ss -> avanti -> indietro = temp_ss -> indietro;
    }

    // Liberazion memoria
    free(temp_mr);
    free(temp_ss);

    printf("Zona cancellata con successo!\n");
    mappa_chiusa = 0;

}

static void stampa_mappa() {
    int scelta;
    printf("\nQuale mappa vuoi stampare?\n");
    printf("1) Mondo Reale\n");
    printf("2) Soprasotto\n");
    printf("Scelta: ");

    if (scanf("%d", &scelta) != 1 || (scelta != 1 && scelta != 2)) {
        printf("Scelta non valida!\n");
        while (getchar() != '\n');
        return;
    }

    if(mappa_chiusa == 0) {
        printf("\nErrore! Mappa non ancora creata!\n");
        return;
    }

    if (scelta == 1) {
        printf("\n=== Mappa Mondo Reale ===\n");
        Zona_mondoreale *temp = prima_zona_mondoreale;
        int i = 1;
        while (temp != NULL) {
            printf("\nZona %d:\n", i);
            printf("  Tipo: %s\n", nome_tipo_zona(temp -> tipo));
            printf("  Nemico: %s\n", nome_tipo_nemico(temp -> nemico));
            printf("  Oggetto: %s\n", nome_tipo_oggetto(temp -> oggetto));
            temp = temp -> avanti;
            i++;
        }
    } else {
        printf("\n=== Mappa Soprasotto ===\n");
        Zona_soprasotto *temp = prima_zona_soprasotto;
        int i = 1;
        while (temp != NULL) {
            printf("\nZona %d:\n", i);
            printf("  Tipo: %s\n", nome_tipo_zona(temp -> tipo));
            printf("  Nemico: %s\n", nome_tipo_nemico(temp -> nemico));
            temp = temp -> avanti;
            i++;
        }
    }

}

static void stampa_zona_specifica() {
    int posizione;
    int n_zone = conta_zone_mondoreale();

    if (n_zone == 0) {
        printf("Non ci sono zone da stampare");
        return;
    }

    printf("\nQUale zona vuoi visualizzare? (1-%d): ", n_zone);
    if (scanf("%d", &posizione) != 1 || posizione < 1 || posizione > n_zone) {
        printf("Posizione non vlida!\n");
        while (getchar() != '\n');
        return;
    }

    // Trova la zona nel Mondo Reale
    Zona_mondoreale *temp_mr = prima_zona_mondoreale;
    for (int i = 1; i < posizione && temp_mr != NULL; i++) {
        temp_mr = temp_mr -> avanti;
    }

    if (temp_mr == NULL) {
        printf("Zona non trovata!");
        return;
    }

    Zona_soprasotto *temp_ss = temp_mr -> link_soprasotto;

    printf("\n=== Zona %d - Mondo Reale ===\n", posizione);
    printf("  Tipo: %s\n", nome_tipo_zona(temp_mr -> tipo));
    printf("  Nemico: %s\n", nome_tipo_nemico(temp_mr -> nemico));
    printf("  Oggetto: %s\n", nome_tipo_oggetto(temp_mr -> oggetto));

    printf("\n=== Zona %d - Soprasotto ===\n", posizione);
    printf("  Tipo: %s\n", nome_tipo_zona(temp_ss -> nemico));
    printf("  Nemico: %s\n", nome_tipo_nemico(temp_ss -> nemico));
}

static void chiudi_mappa() {
    int n_zone =  conta_zone_mondoreale();
    int n_demotorzone = conta_demotorzone();

    if (n_zone <15) {
        printf("Errore! ci devono essere almeno 15 zone. Attualmente ce ne sono %d.\n", n_zone);
        return;
    }

    if (n_demotorzone != 1) {
        printf("Errore! Dev esserci esattamente 1 Demotorzone. Attalemente ce ne sono %d.\n", n_demotorzone);
        return;
    }

    mappa_chiusa = 1;
    printf("Mappa chiusa con successo!\n");
    return;
}

// Funzione imposta_gioco

void imposta_gioco() {
    printf("\n=== Impostazione Gioco ===\n");

    // Dallocazione se il gioco era gia impostato
    if (gioco_impostato) {
        printf("Reimpostzione del gioco in corso...\n");
        dealloca_gioctori();
        dealloca_mondoreale();
        dealloca_soprasotto();
        mappa_chiusa = 0;
        gioco_impostato = 0;
    }

    // Numero giocatori
    printf("Quanti giocatori siete? (1-4): ");
    if (scanf("%d", &n_giocatori) != 1 || n_giocatori < 1 || n_giocatori > 4) {
        printf("Numero di giocatori non valido!\n");
        while (getchar() != '\n');
        n_giocatori = 0;
        return;
    }
    while (getchar() != '\n');

    // Variabile check UndiciVirgolaCinque
    int undici_virgola_cinque_scelto = 0;

    // Creazionee gioctori
    for (int i = 0; i < n_giocatori; i++){
        giocatori[i] = (Giocatore*)malloc(sizeof(Giocatore));
        if (giocatori[i] == NULL) {
            printf("Errore allocaazione memoria!\n");
            return;
        }

        printf("\n--- Giocatore %d ---\n", i + 1);
        printf("Inserisci il tuo nome: ");
        fgets(giocatori[i] -> nome_giocatore, 50, stdin);
        giocatori[i] -> nome_giocatore [strcspn(giocatori[i] -> nome_giocatore, "\n")] = 0;
        
        // Lancio dei dadi per le abilita
        giocatori[i] -> attaco_psichico = lancia_dado(20);
        giocatori[i] -> difesa_psichica = lancia_dado(20);
        giocatori[i] -> fortuna = lancia_dado(20);
    
        printf("Abilita Iniziali: \n");
        printf("  Attacco Psichico: %d\n", giocatori[i] -> attaco_psichico);
        printf("  Difesa Psichica: %d\n", giocatori[i] -> difesa_psichica);
        printf("  Fortuna: %d\n", giocatori[i] -> fortuna);
    
        // Possibilita di modifica stat
        printf("\nVuoi modificare le tue abilita?\n");
        printf("1)   Attacco (+3) e diminuisci Difesa (-3)\n");
        printf("2)   Difesa (+3) e diminuisci Attacco (-3)\n");

        if (!undici_virgola_cinque_scelto) {
            printf("3) Diventa UndiciVirgolaCinque! (+4 Attacco, +4 Difesa, -7 Fortuna)\n");
        }

        printf("0) Nessuna modifica\n");
        printf("Scelta: ");

        int scelta;
        if (scanf("%d", &scelta) != 1) {
            printf("Input non valido, nessuna modifica applicata.\n");
            while (getchar() != '\n');
            scelta = 0;
        } 

        while (getchar() != '\n');

        if (scelta == 1) {
            giocatori[i] -> attaco_psichico += 3;
            giocatori[i] -> difesa_psichica -=3;
            printf("Modifiche applicate!\n");
        } else if (scelta == 2) {
            giocatori[i] -> attaco_psichico -= 3;
            giocatori[i] -> difesa_psichica +=3;
            printf("Modifiche applicate!\n");
        } else if (scelta == 3 && !undici_virgola_cinque_scelto) {
            giocatori[i] -> attaco_psichico += 4;
            giocatori[i] -> difesa_psichica +=4;
            giocatori[i] -> fortuna -= 7;
            strcpy(giocatori[i] -> nome_giocatore, "UndiciVirgolaCinque");
            undici_virgola_cinque_scelto = 1;
            printf("Sei divntato UndiciVirgolaaCinque!\n");
        }

        // Inizializzazione zaino vuoto
        for (int j = 0; j <3; j++) {
            giocatori[i] -> zaino[j] = nessun_oggetto;
        }

        // Inizializzazione posizoni
        giocatori[i] -> mondo = 0; // Per iniziare nel Mondo Reale
        giocatori[i] -> pos_mondoreale = NULL; // Impostato a inizio prtita
        giocatori[i] -> pos_soprasotto = NULL;
    }

    // Menu creaazione mappa
    printf("\n=== Creazione Mappa ===\n");
    int scelta_mappa;

    do {
        printf("\n1) Genera mappa automatica (15 zone)\n");
        printf("2) Inserisci zona manualmetne\n");
        printf("3) Cancella zona\n");
        printf("4) Stampa mappa\n");
        printf("5) Stampa zona specifica\n");
        printf("6) Chiudi mappa e termina impostazione\n");
        printf("Scelta: ");

        if (scanf("%d", &scelta_mappa) != 1) {
            printf("Input non valido!\n");
            while (getchar() != '\n');
            continue;
        }

        switch (scelta_mappa) {
            case 1:
                genera_mappa();
                break;
            case 2:
                inserisci_zona();
                break;
            case 3:
                cancella_zona();
                break;
            case 4:
                stampa_mappa();
                break;
            case 5:
                stampa_zona_specifica();
                break;
            case 6:
                chiudi_mappa();
                if (mappa_chiusa) {
                    gioco_impostato = 1;
                    printf("\nGioco impostato con successo!\n");
                }
                break;
            default:
                printf("Scelta non valida!\n");
        }
    } while (scelta_mappa != 6 || !mappa_chiusa);

}

// Funzioni di gioco

static void stampa_giocatore_corrente(int indice) {
    if (giocatori[indice] == NULL) {
        printf("Giocatore morto!\n");
        return;
    }

    Giocatore *g = giocatori[indice];
    printf("\n=== %s ===\n", g -> nome_giocatore);
    printf("Mondo: %s\n", g -> mondo == 0 ? "Mondo Reale" : "Sopraasotto");
    printf("Attacco Psichico: %d\n", g -> attaco_psichico);
    printf("Difesa Psichica: %d\n", g -> difesa_psichica);
    printf("Fortuna: %d\n", g -> fortuna);
    printf("Zaino:");
    int vuoto = 1;
    for (int i = 0; i < 3; i++) {
        if (g -> zaino[i] != nessun_oggetto) {
            printf("%s ", nome_tipo_oggetto(g -> zaino[i]));
            vuoto = 0;
        }
    }

    if (vuoto) printf("(vuoto)");
    printf("\n");
}


static void stampa_zona_corrente(int indice) {
    if (giocatori[indice] == NULL) return;

    Giocatore *g = giocatori[indice];

    printf("\n=== Zona Attuale ===\n");
    if (g -> mondo == 0 && g -> pos_mondoreale != NULL) {
    printf("Tipo: %s\n", nome_tipo_zona(g -> pos_mondoreale -> tipo));
    printf("Nemico: %s\n", nome_tipo_nemico(g -> pos_mondoreale -> nemico));
    printf("Oggetto: %s\n", nome_tipo_oggetto(g -> pos_mondoreale -> oggetto));
    } else if (g -> mondo == 1 && g -> pos_soprasotto != NULL) {
    printf("Tipo: %s\n", nome_tipo_zona(g -> pos_soprasotto -> tipo));
    printf("Nemico: %s\n", nome_tipo_nemico(g -> pos_soprasotto-> nemico));
    }

}


static int combatti_nemico(int indice, Tipo_nemico nemico, int *bonus_attacco_temp) {
    if (nemico == nessun_nemico) {
        return 1;  // Nessun combattimento
    }
    
    Giocatore *g = giocatori[indice];
    printf("\n=== COMBATTIMENTO contro %s ===\n", nome_tipo_nemico(nemico));
    
    // Statistiche del nemico (da personalizzare)
    int vita_nemico, attacco_nemico, difesa_nemico;
    
    switch (nemico) {
        case billi:
            vita_nemico = 20;
            attacco_nemico = 8;
            difesa_nemico = 5;
            break;
        case democane:
            vita_nemico = 30;
            attacco_nemico = 12;
            difesa_nemico = 8;
            break;
        case demotorzone:
            vita_nemico = 50;
            attacco_nemico = 18;
            difesa_nemico = 12;
            break;
        default:
            return 1;
    }
    
    int vita_giocatore = 50;  // Vita base del giocatore
    
    printf("Il combattimento inizia!\n");
    printf("Vita giocatore: %d | Vita nemico: %d\n", vita_giocatore, vita_nemico);
    
    // Combattimento a turni
    while (vita_giocatore > 0 && vita_nemico > 0) {
        printf("\n1) Attacca\n");
        printf("2) Difendi\n");
        printf("3) Usa oggetto\n");
        printf("Scelta: ");
        
        int scelta;
        if (scanf("%d", &scelta) != 1) {
            printf("Input non valido!\n");
            while (getchar() != '\n');
            continue;
        }
        
        if (scelta == 1) {
            // Attacco del giocatore
            int dado_attacco = lancia_dado(20);
            int danno = g -> attaco_psichico + dado_attacco - difesa_nemico;
            if (danno < 0) danno = 0;
            
            vita_nemico -= danno;
            printf("Hai inflitto %d danni! (Dado: %d)\n", danno, dado_attacco);
            
            if (vita_nemico <= 0) {
                printf("\n*** HAI VINTO! ***\n");
                
                // 50% di probabilità che il nemico scompaia
                if (rand() % 2 == 0) {
                    printf("Il nemico è scomparso dalla zona!\n");
                    if (g->mondo == 0 && g->pos_mondoreale != NULL) {
                        g->pos_mondoreale->nemico = nessun_nemico;
                    } else if (g->mondo == 1 && g->pos_soprasotto != NULL) {
                        g->pos_soprasotto->nemico = nessun_nemico;
                    }
                }
                
                // Controlla se era il Demotorzone
                if (nemico == demotorzone) {
                    printf("\n*** HAI SCONFITTO IL DEMOTORZONE! ***\n");
                    printf("*** %s VINCE LA PARTITA! ***\n", g->nome_giocatore);
                    
                    // Salva il vincitore
                    if (n_vincitori < 3) {
                        strcpy(vincitori_precedenti[n_vincitori], g->nome_giocatore);
                        n_vincitori++;
                    } else {
                        // Sposta i vincitori precedenti
                        strcpy(vincitori_precedenti[0], vincitori_precedenti[1]);
                        strcpy(vincitori_precedenti[1], vincitori_precedenti[2]);
                        strcpy(vincitori_precedenti[2], g->nome_giocatore);
                    }
                    
                    return 2;  // Vittoria!
                }
                
                return 1;  // Combattimento vinto
            }
            
        } else if (scelta == 2) {
            printf("Ti difendi!\n");
        } else if (scelta == 3) {
            int esito_oggetto = utilizza_oggetto_giocatore(indice, bonus_attacco_temp);
            if (esito_oggetto == 0) {
                return 0;
            }
            if (esito_oggetto == 2) {
                return 2;
            }
            scelta = 0;
        }
        
        // Turno del nemico
        if (vita_nemico > 0) {
            int dado_nemico = lancia_dado(20);
            int danno_nemico = attacco_nemico + dado_nemico - g->difesa_psichica;
            if (danno_nemico < 0) danno_nemico = 0;
            
            if (scelta == 2) {
                danno_nemico /= 2;  // Dimezza il danno con la difesa
            }
            
            vita_giocatore -= danno_nemico;
            printf("Il nemico ti infligge %d danni! (Dado: %d)\n", danno_nemico, dado_nemico);
            printf("Vita giocatore: %d | Vita nemico: %d\n", vita_giocatore, vita_nemico);
            
            if (vita_giocatore <= 0) {
                printf("\n*** SEI MORTO! ***\n");
                free(giocatori[indice]);
                giocatori[indice] = NULL;
                return 0;  // Giocatore morto
            }
        }
    }
    return 1;

}


// Movimento

static int avanza_giocatore(int indice, int *bonus_attacco_temp) {
    if (giocatori[indice] == NULL) return 0;
    
    Giocatore *g = giocatori[indice];
    
    // Combatti il nemico nella zona corrente se presente
    Tipo_nemico nemico_corrente = nessun_nemico;
    
    if (g->mondo == 0 && g->pos_mondoreale != NULL) {
        nemico_corrente = g->pos_mondoreale->nemico;
    } else if (g->mondo == 1 && g->pos_soprasotto != NULL) {
        nemico_corrente = g->pos_soprasotto->nemico;
    }
    
    int risultato_combattimento = combatti_nemico(indice, nemico_corrente, bonus_attacco_temp);
    
    if (risultato_combattimento == 0) {
        return 0;  // Giocatore morto
    }
    
    if (risultato_combattimento == 2) {
        return 2;  // Vittoria!
    }
    
    // Avanza alla zona successiva
    if (g->mondo == 0) {
        if (g->pos_mondoreale != NULL && g->pos_mondoreale->avanti != NULL) {
            g->pos_mondoreale = g->pos_mondoreale->avanti;
            printf("Sei avanzato alla zona successiva!\n");
        } else {
            printf("Non puoi avanzare oltre!\n");
        }
    } else {
        if (g->pos_soprasotto != NULL && g->pos_soprasotto->avanti != NULL) {
            g->pos_soprasotto = g->pos_soprasotto->avanti;
            printf("Sei avanzato alla zona successiva!\n");
        } else {
            printf("Non puoi avanzare oltre!\n");
        }
    }
    
    return 1;
}

static int indietreggia_giocatore(int indice, int *bonus_attacco_temp) {
    if (giocatori[indice] == NULL) return 0;
    
    Giocatore *g = giocatori[indice];
    
    // Combatti il nemico nella zona corrente se presente
    Tipo_nemico nemico_corrente = nessun_nemico;
    
    if (g->mondo == 0 && g->pos_mondoreale != NULL) {
        nemico_corrente = g->pos_mondoreale->nemico;
    } else if (g->mondo == 1 && g->pos_soprasotto != NULL) {
        nemico_corrente = g->pos_soprasotto->nemico;
    }
    
    int risultato_combattimento = combatti_nemico(indice, nemico_corrente, bonus_attacco_temp);
    
    if (risultato_combattimento == 0) {
        return 0;  // Giocatore morto
    }
    
    if (risultato_combattimento == 2) {
        return 2;  // Vittoria!
    }
    
    // Indietreggia alla zona precedente
    if (g->mondo == 0) {
        if (g->pos_mondoreale != NULL && g->pos_mondoreale->indietro != NULL) {
            g->pos_mondoreale = g->pos_mondoreale->indietro;
            printf("Sei indietreggiato alla zona precedente!\n");
        } else {
            printf("Non puoi indietreggiare oltre!\n");
        }
    } else {
        if (g->pos_soprasotto != NULL && g->pos_soprasotto->indietro != NULL) {
            g->pos_soprasotto = g->pos_soprasotto->indietro;
            printf("Sei indietreggiato alla zona precedente!\n");
        } else {
            printf("Non puoi indietreggiare oltre!\n");
        }
    }
    
    return 1;
}



// Passaggio tra mondi
static int cambia_mondo_giocatore(int indice, int avanza_gia_chiamata) {
    if (giocatori[indice] == NULL) return 0;
    
    Giocatore *g = giocatori[indice];
    
    if (avanza_gia_chiamata) {
        printf("Non puoi cambiare mondo se hai già avanzato in questo turno!\n");
        return 1;
    }
    
    // Se sei nel Mondo Reale
    if (g->mondo == 0) {
        // Devi aver sconfitto il nemico
        if (g->pos_mondoreale != NULL && g->pos_mondoreale->nemico != nessun_nemico) {
            printf("Devi prima sconfiggere il nemico!\n");
            return 1;
        }
        
        // Cambia nel Soprasotto
        g->mondo = 1;
        if (g->pos_mondoreale != NULL) {
            g->pos_soprasotto = g->pos_mondoreale->link_soprasotto;
        }
        printf("Sei entrato nel Soprasotto!\n");
        
    } else {
        // Sei nel Soprasotto, prova a tornare nel Mondo Reale
        int dado_fortuna = lancia_dado(20);
        printf("Tiri il dado della fortuna... %d\n", dado_fortuna);
        
        if (dado_fortuna < g->fortuna) {
            printf("Ce l'hai fatta! Torni nel Mondo Reale!\n");
            g->mondo = 0;
            if (g->pos_soprasotto != NULL) {
                g->pos_mondoreale = g->pos_soprasotto->link_mondoreale;
            }
        } else {
            printf("Non sei abbastanza fortunato! Rimani nel Soprasotto.\n");
        }
    }
    
    return 1;
}


// Funzioni zaino
static void raccogli_oggetto_giocatore(int indice) {
    if (giocatori[indice] == NULL) return;
    
    Giocatore *g = giocatori[indice];
    
    // Solo nel Mondo Reale ci sono oggetti
    if (g->mondo != 0 || g->pos_mondoreale == NULL) {
        printf("Non ci sono oggetti in questa zona!\n");
        return;
    }
    
    // Controlla se c'è un nemico
    if (g->pos_mondoreale->nemico != nessun_nemico) {
        printf("Devi prima sconfiggere il nemico!\n");
        return;
    }
    
    // Controlla se c'è un oggetto
    if (g->pos_mondoreale->oggetto == nessun_oggetto) {
        printf("Non c'è nessun oggetto in questa zona!\n");
        return;
    }
    
    // Cerca spazio nello zaino
    for (int i = 0; i < 3; i++) {
        if (g->zaino[i] == nessun_oggetto) {
            g->zaino[i] = g->pos_mondoreale->oggetto;
            printf("Hai raccolto: %s\n", nome_tipo_oggetto(g->pos_mondoreale->oggetto));
            g->pos_mondoreale->oggetto = nessun_oggetto;
            return;
        }
    }
    
    printf("Lo zaino è pieno!\n");
}


// Uso oggetto
static int utilizza_oggetto_giocatore(int indice, int *bonus_attacco_temp) {
    if (giocatori[indice] == NULL) return;
    
    Giocatore *g = giocatori[indice];
    
    printf("\nZaino:\n");
    int ha_oggetti = 0;
    for (int i = 0; i < 3; i++) {
        if (g->zaino[i] != nessun_oggetto) {
            printf("%d) %s\n", i + 1, nome_tipo_oggetto(g->zaino[i]));
            ha_oggetti = 1;
        }
    }
    
    if (!ha_oggetti) {
        printf("Lo zaino è vuoto!\n");
        return;
    }
    
    printf("Quale oggetto vuoi usare? (1-3, 0 per annullare): ");
    int scelta;
    if (scanf("%d", &scelta) != 1 || scelta < 0 || scelta > 3) {
        printf("Scelta non valida!\n");
        while (getchar() != '\n');
        return;
    }
    
    if (scelta == 0) return;
    
    if (g->zaino[scelta - 1] == nessun_oggetto) {
        printf("Non c'è nessun oggetto in quella posizione!\n");
        return;
    }

    int risultato = 1;
    
    // Effetti degli oggetti (da aaggiungere)
    switch (g->zaino[scelta - 1]) {
        case bicicletta:
            printf("Usi la bicicletta! Puoi avanzare di una zona extra!\n");
            risultato = avanza_giocatore(indice, bonus_attacco_temp);
            break;
        case maglietta_fuocoinferno:
            printf("Indossi la Maglietta Fuocoinferno! Attacco +5 per questo turno!\n");
            g -> attaco_psichico += 5;
            if (bonus_attacco_temp != NULL) {
                *bonus_attacco_temp += 5;
            }
            break;
        case bussola:
            printf("Consulti la bussola! Fortuna +3!\n");
            g->fortuna += 3;
            break;
        case schitarrata_metallica:
            printf("Suoni la Schitarrata Metallica! Il nemico nella zona fugge!\n");
            if (g->mondo == 0 && g->pos_mondoreale != NULL) {
                g->pos_mondoreale->nemico = nessun_nemico;
            } else if (g->mondo == 1 && g->pos_soprasotto != NULL) {
                g->pos_soprasotto->nemico = nessun_nemico;
            }
            break;
        default:
            break;
    }
    
    // Rimuovi l'oggetto dallo zaino
    if (giocatori[indice] != NULL) {
        g->zaino[scelta - 1] = nessun_oggetto;
    }

}


// Funzione gioca
void gioca() {
    if (!gioco_impostato || !mappa_chiusa) {
        printf("Errore! Devi prima impostare il gioco correttamente!\n");
        return;
    }
    
    printf("\n=== INIZIO PARTITA ===\n");
    
    // Posiziona tutti i giocatori nella prima zona del Mondo Reale
    for (int i = 0; i < n_giocatori; i++) {
        if (giocatori[i] != NULL) {
            giocatori[i]->mondo = 0;
            giocatori[i]->pos_mondoreale = prima_zona_mondoreale;
            giocatori[i]->pos_soprasotto = NULL;
        }
    }
    
    // Array per l'ordine di gioco
    int ordine[4];
    int giocatori_vivi = n_giocatori;
    
    // Loop di gioco
    while (giocatori_vivi > 0) {
        // Genera ordine casuale
        int temp_ordine[4];
        int count = 0;
        
        for (int i = 0; i < n_giocatori; i++) {
            if (giocatori[i] != NULL) {
                temp_ordine[count++] = i;
            }
        }
        
        // Mescola l'ordine (Fisher-Yates shuffle)
        for (int i = count - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            int temp = temp_ordine[i];
            temp_ordine[i] = temp_ordine[j];
            temp_ordine[j] = temp;
        }
        
        // Copia nell'ordine finale
        for (int i = 0; i < count; i++) {
            ordine[i] = temp_ordine[i];
        }
        
        // Turni dei giocatori
        for (int i = 0; i < count; i++) {
            int indice = ordine[i];
            
            if (giocatori[indice] == NULL) continue;
            
            printf("\n========================================\n");
            printf("TURNO DI: %s\n", giocatori[indice] -> nome_giocatore);
            printf("========================================\n");
            
            stampa_giocatore_corrente(indice);
            stampa_zona_corrente(indice);
            
            int avanza_chiamata = 0;
            int turno_finito = 0;
            int bonus_attacco_temp = 0;
            
            while (!turno_finito) {
                printf("\n1) Avanza\n");
                printf("2) Indietreggia\n");
                printf("3) Cambia mondo\n");
                printf("4) Raccogli oggetto\n");
                printf("5) Usa oggetto\n");
                printf("6) Stampa giocatore\n");
                printf("7) Stampa zona\n");
                printf("8) Passa il turno\n");
                printf("Scelta: ");
                
                int scelta;
                if (scanf("%d", &scelta) != 1) {
                    printf("Input non valido!\n");
                    while (getchar() != '\n');
                    continue;
                }
                
                int risultato;
                
                switch (scelta) {
                    case 1:
                        if (avanza_chiamata) {
                            printf("Hai già avanzato in questo turno!\n");
                        } else {
                            risultato = avanza_giocatore(indice, &bonus_attacco_temp);
                            if (risultato == 2) {
                                printf("\n*** PARTITA TERMINATA! ***\n");
                                dealloca_gioctori();
                                dealloca_mondoreale();
                                dealloca_soprasotto();
                                gioco_impostato = 0;
                                mappa_chiusa = 0;
                                return;  // Vittoria!
                            }
                            if (risultato == 0) {
                                giocatori_vivi--;
                                turno_finito = 1;
                            } else {
                                avanza_chiamata = 1;
                            }
                        }
                        break;
                        
                    case 2:
                        if (avanza_chiamata) {
                            printf("Hai già mosso in questo turno!\n");
                        } else {
                            risultato = indietreggia_giocatore(indice, &bonus_attacco_temp);
                            if (risultato == 2) {
                                printf("\n*** PARTITA TERMINATA! ***\n");
                                dealloca_gioctori();
                                dealloca_mondoreale();
                                dealloca_soprasotto();
                                gioco_impostato = 0;
                                mappa_chiusa = 0;
                                return;
                            }
                            if (risultato == 0) {
                                giocatori_vivi--;
                                turno_finito = 1;
                            } else {
                                avanza_chiamata = 1;
                            }
                        }
                        break;
                        
                    case 3:
                        risultato = cambia_mondo_giocatore(indice, avanza_chiamata);
                        if (risultato == 0) {
                            giocatori_vivi--;
                            turno_finito = 1;
                        }
                        break;
                        
                    case 4:
                        raccogli_oggetto_giocatore(indice);
                        break;
                        
                    case 5:
                        utilizza_oggetto_giocatore(indice, &bonus_attacco_temp);
                        break;
                        
                    case 6:
                        stampa_giocatore_corrente(indice);
                        break;
                        
                    case 7:
                        stampa_zona_corrente(indice);
                        break;
                        
                    case 8:
                        printf("Passi il turno.\n");
                        turno_finito = 1;
                        break;
                        
                    default:
                        printf("Scelta non valida!\n");
                }
            }

            if (bonus_attacco_temp != 0 && giocatori[indice] != NULL) {
                giocatori[indice] -> attaco_psichico -= bonus_attacco_temp;
            }

            }

        }
        
        // Controlla se tutti i giocatori sono morti
        giocatori_vivi = 0;
        for (int i = 0; i < n_giocatori; i++) {
            if (giocatori[i] != NULL) {
                giocatori_vivi++;
            }
        }
        
        if (giocatori_vivi == 0) {
            printf("\n*** TUTTI I GIOCATORI SONO MORTI! GAME OVER! ***\n");
            dealloca_gioctori();
            dealloca_mondoreale();
            dealloca_soprasotto();
            gioco_impostato = 0;
            mappa_chiusa = 0;
            return;
        }
    }

}


// Funzioni di fine gioco

void termina_gioco() {
    printf("\nGrazie per avr giocato a Cosestrane!\n");

    // Deallocazione memoria
    dealloca_gioctori();
    dealloca_mondoreale();
    dealloca_soprasotto();
    gioco_impostato = 0;
    mappa_chiusa = 0;

}


void crediti() {
    printf("\n=== Crediti ===\n");
    printf("Gioco creato da: Filippo Castagnola N. Matricola: 376476\n");

    if (n_vincitori > 0) {
        printf("\n--- Ultimi Vincitiori ---\n");
        for (int i = 0; i < n_vincitori; i++) {
            printf("%d. %s\n", i + 1, vincitori_precedenti[i]);
        }
    } else {
        printf("\nAncora nessun vincitore");
    }

}
