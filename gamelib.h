// FUnzioni main

    void imposta_gioco();
    void gioca();
    void termina_gioco();
    void crediti();

    void verifica_errori();

// Definizioni Tipi

typedef enum {
    bosco, scuola, laboratorio, caverna, strada, giardino, supermercato, 
    centrale_elettrica, deposito_abbandonato, stazione_polizia
} Tipo_zona;

typedef enum {
    nessun_nemico, billi, democane, demotorzone
} Tipo_nemico;

typedef enum {
    nessun_oggetto, bicicletta, 
    maglietta_fuocoinferno, bussola, schitarrata_metallica
} Tipo_oggetto;

struct Zona_mondoreale;
struct Zona_soprasotto;

    typedef struct Giocatore {

        char nome_giocatore[20];
        int mondo; // 0 = reale, 1 = soprasotto
        struct Zona_mondoreale *pos_mondoreale;
        struct Zona_mondoreale *pos_soprasotto;
        int attaco_psichico;
        int difesa_psichica;
        int fortuna;
        Tipo_oggetto zaino[3];
    } Giocatore;
    
    typedef struct Zona_mondoreale {

        Tipo_zona tipo;
        Tipo_nemico nemico;
        Tipo_oggetto oggetto;
        struct Zona_mondoreale *avanti;
        struct Zona_mondoreale *indietro;
        struct Zona_soprasotto *link_soprasotto;
    } Zona_mondoreale;

    typedef struct Zona_soprasotto {

        Tipo_zona tipo;
        Tipo_nemico nemico;
        Tipo_oggetto oggetto;
        struct Zona_soprasotto *avanti;
        struct Zona_soprasotto *indietro;
        struct Zona_mondoreale *link_mondoreale;
    } Zona_soprasotto;


