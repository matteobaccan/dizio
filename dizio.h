
#define LINE_LENGHT 100           // Lunghezza massima della singola parola
#define TEXTMEMORY  10000         // Massima memoria allocabile per il testo
#define DIZIONARIO  "DIZIO.TXT"

typedef struct dizionario{
          char parola[LINE_LENGHT];  // Dich. della struttura  (lista doppia)
          struct dizionario * dopo;  // che conterra` i vocaboli.
          struct dizionario * prima;
          }lista;


lista * Carica_Dizionario( lista * testa ); // Carica dizionario

lista * Carica_Lista( char * fpBuffer ,
                      lista * testa   ); // Mette in memoria la lista

lista * Carica_Testo( char * fileTesto ,
                      char * bigBuffer ,
                      lista * testa    ); // Carica il testo da controllare

lista * Controllo_Parola( char * fpToken   ,  // Controlla che la parola sia "accettabile"
                          lista * testa    ,
                          FILE* fpFile     ,
                          char * bigBuffer ,
                          char * p         );

lista * Controlla_Dizio( lista * testa    , // verifica la presenza della parola in lista
                         char * fpToken   ,
                         char * bigBuffer ,
                         char * p         );

lista * In_testa( lista * nuovo   ,  // Realizzano l'inserimento
                  lista * attuale );

void In_mezzo( lista * nuovo   ,     // nella lista
               lista * attuale );

void In_coda( lista * nuovo   ,
              lista * attuale );

lista * Primo_ins( lista * nuovo   ,
                   lista * attuale );

char menu( char * vocabolo );  // Riceve le scelte dell'utente

lista * Correggi( lista * testa    ,
                  char * vocabolo  ,
                  char * p         ,
                  char * bigBuffer ); // Gestisce eventuali correzione

