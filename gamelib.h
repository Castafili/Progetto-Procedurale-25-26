// FUnzioni main

    void imposta_gioco();
    void gioca();
    void termina_gioco();
    void crediti();

    void verifica_errori();

// Definizioni Tipi

typedef enum {
    bosco, scuola, labortorio, cavern, strada, giardino, supermercato, 
    centrale_elettrica, deposito_abbandonato, stazione_polizia
} Tipo_zona;

typedef enum {
    nessun_nemico, billi, democane, demotorzone
} Tipo_nemico;

typedef enum {
    nessun_oggetto, bicicletta, 
    maglietta_fuocoinferno, bussola, schitarrata_metallica
} Tipo_oggetto;

    typedef struct Giocatore {

        char nome_giocatore[20];
        int mondo; // 0 = reale, 1 = soprasotto
        int *pos_mondoreale;
        int *pos_soprasotto;
        int attaco_psichico;
        int difesa_psichica;
        int fortuna;
        Tipo_oggetto zaino[3];
        } Giocatore;
    
    typedef struct Zona_mondoreale {
        
    } Mondoreale;

    typedef struct Zona_soprasotto {

    } Soprasotto;


