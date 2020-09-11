/*
 * Dizio 1.0
 *
 * Copyright 2000 Matteo Baccan <mbaccan@planetisa.com>
 * www - http://www.infomedia.it/artic/Baccan
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA (or visit
 * their web site at http://www.gnu.org/).
 *
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <stdlib.h>

#include "dizio.h" // Include locale

void main(void){

    lista * testa;
    char str[LINE_LENGHT],*bigBuffer,*t;
    FILE *fpFile1,*fpFile2;                           // Alloca
                                                       // la memoria
    clrscr();                                           // del file di testo
    if ((bigBuffer=(char *)malloc(TEXTMEMORY))==NULL){
       printf(" 1 Impossibile continuare per mancanza di memoria");
       gets(t);
       exit(0);
    }
    bigBuffer[0]=NULL;
    testa=NULL;
                                      // Legge il file dizionario,
                                      // lo mette in memoria e lo
    testa=Carica_Dizionario( testa ); // spezza in tokens che poi
                                      // passa al controllo parola

                                                        // Acquisisce il
    printf("Nome completo del file da controllare:  "); // nome file da
    gets(str);                                          // controllare

    testa =Carica_Testo( str, bigBuffer, testa );   // Mette in memoria il file da contr.

    if ((fpFile1=fopen( DIZIONARIO, "w"))!=NULL ){
       while( testa!=NULL ){
          fputs( testa->parola, fpFile1 );
          fputs( "\n",fpFile1 );             // File del dizionario
          testa = testa->dopo;
       }fclose (fpFile1);
    }else printf ("Non riesco ad aprire il file del dizionario");

    // Scrivere ultimo buffer in append
    // chiudi
    // cancellare REMOVE l'originale
    // rinomini RENAME il bigbuffer su disco

    // Trasferimenti su disco
    if ((fpFile2=fopen( str, "w"))!=NULL ){
       fputs( bigBuffer,fpFile2 );
       fputs(NULL,fpFile2);
    }fclose (fpFile2);             // File del testo corretto

    free(bigBuffer);

}



// CARICA DIZIONARIO
// Apre da disco il file del dizionario e tramite un buffer (array)
// lo mette in memoria passando ciascuna parola ogni volta alla funzione
// che la carica in lista. Elimina il tasto di ritorno a capo

lista* Carica_Dizionario(lista * testa)
{
    FILE * fpFile;
    char fpBuffer[LINE_LENGHT+1], *t;

    if ((fpFile= fopen(DIZIONARIO ,"r"))== NULL){
       fprintf(stderr, "E` impossibile aprire il dizionario\n");
       gets(t);
       exit(0);
    }

    while(!feof(fpFile)){
       while (fgets( fpBuffer, LINE_LENGHT, fpFile )!=NULL){
          if (fpBuffer[strlen(fpBuffer)-1]=='\n') // 1) elimina il carriage return (\n)
             fpBuffer[strlen(fpBuffer)-1]=NULL;   // 2) lo rimpiazza con il terminatore
             testa=Carica_Lista( fpBuffer, testa);// 3)passa a carica listaogni parola
       }
    }fclose( fpFile );// Chiusura del file dizionario ormai caricato in memoria

    return (testa);
}


// CARICA LISTA
// Riceve due parametri: la testa della lista e il puntatore a carattere
// che individua il vocabolo da inserire. Distingue il caso del primo inserimento
// da quello in cui la testa non e` "NULL" a seconda del psizionamento in lista.
//
lista * Carica_Lista( char * fpBuffer, lista * testa)
{
    char *t;
    lista * nuovo, *attuale;

    attuale=testa;
    if ((nuovo=(lista *) malloc( sizeof(lista)))==NULL ){  // Viene allocata
       printf("\nNon c'e` piu` memoria disponibile!\n");   // la memoria necessaria
       gets(t);
       exit (1);                                           // per il nuovo vocabolo
    }
    strcpy( nuovo->parola, fpBuffer );
    if (testa==NULL ){
       testa=Primo_ins(nuovo, attuale);     // Primo inserimento il lista
       return (testa);
    }
						     // Ora la testa e` diversa da NULL
    while (((strcmp(attuale->parola,fpBuffer))<0) && // Si individua il punto
		   (attuale->dopo!=NULL))            // in cui inserire il vocabolo
       attuale=attuale->dopo;

    if ((strcmp(attuale->parola,fpBuffer)==0))
       return (testa);     // La parola c'e` gia`: ritorna al chiamante.

    if ((strcmp(attuale->parola,fpBuffer))>0){
       if (attuale->prima==NULL){
          testa=In_testa(nuovo,attuale);  // Inserimento in testa alla lista
       } else
          In_mezzo(nuovo,attuale); // Inserimento  in centro lista
    } else
        In_coda(nuovo,attuale);    // Inserimento  in coda lista

    return (testa);
}




//  CARICA TESTO
// Apre il file contenente il testo da controllare; lo suddivide
// in token (secondo i separatori specificati) e chiama la funzione
// Controllo_Parola su ogni token (parola) individuato, fino a fine-file.
// Contestualmente provvede a riempire il Buffer del testo  residente in memoria.

lista *Carica_Testo(char * fileTesto,
                         char * bigBuffer, lista * testa)
{
    FILE * fpFile;
    char fpBuffer[LINE_LENGHT+1];
    char * fpToken,*p,*t;

    if ((fpFile=fopen( fileTesto, "r" ))==NULL ){
       printf("\nErrore in fase di apertura del file indicato/n");
       exit(2);
       return (testa);
    }
                                                          // Legge "LINE_LENGHT" caratteri
    while((fgets( fpBuffer, LINE_LENGHT, fpFile))!=NULL){ // dal file.
       clrscr();
       printf ("\nSTRINGA IN FASE DI CONTROLLO:\n ----->   ");
       puts(fpBuffer);
       printf("\n\n\n");
       strcat(bigBuffer,fpBuffer);
       if ( (strlen(bigBuffer)) > (TEXTMEMORY-LINE_LENGHT)){
          printf("Il testo e` troppo grande!\n Premere un tasto");
          gets(t);
          exit(0);
       }
       if ((fpToken=strtok( fpBuffer," ,.;:\'\n\"()?!" ))!=NULL){// Suddivide in token
          p=strstr(bigBuffer, fpToken);                          // la stringa letta
          testa = Controllo_Parola(fpToken, testa,fpFile,  bigBuffer, p); // e li passa
       }                                                         //uno alla volta al
       while ((fpToken=strtok(NULL," ,.;:\'\n\"()?!"))!=NULL){  // controllo parola.
          p=strstr(p, fpToken);
          testa=Controllo_Parola(fpToken, testa,fpFile,bigBuffer, p);
       }                                                // "p" servira` nella
    }fclose( fpFile );                                  // funzione Correggi.

    return (testa);
}


// CONTROLLO PAROLA
// "Tratta" la stringa in entrata (= vocabolo= token di turno) per
// epurarla dai caratteri che ne inficierebbero il controllo (trattino,
// accenti, numeri, punteggiature diverse) o scartarla del tutto.


lista * Controllo_Parola( char * vocabolo, lista * testa,
                          FILE* fpFile,char * bigBuffer, char *p)
{
    int j,i=strlen(vocabolo);
    char c,a[LINE_LENGHT];

    if (vocabolo[i-1]=='-'){
       j=0;                      // Gestisce la presenza del trattino di divisione
       do{
          c=getc(fpFile);        // parola, legge il file di testo per ricomporre
          a[j]=c;
          vocabolo[i-1]=c;       // il vocabolo originale. Lo spostamento del puntatore
          i++;
          j++;                   // al file richiede l'aggiornamento del Buffer di testo
       }while( (isalpha(c)) || c=='`');
       vocabolo[i-2]=NULL;              // e l'eliminazione di un token inutile.
       strtok(NULL," ,.;:\'\n\"()?!");
       a[j]=NULL;
       strcat(bigBuffer,a);
    }else{
        for(j=0;j<i;j++){                                 // Numeri e punteggiature
           if ( !isalpha(vocabolo[j]) && vocabolo[j]!='`')// (diverse da "-" e "`",
                                                          // impediscono che avvenga il
                 return(testa);                           // confronto con la lista.
        }
    }
    return(Controlla_Dizio(testa,strlwr(vocabolo),bigBuffer, p));                                 // da eventuali caratteri non accettati,
}
                                                            // ogni parola e` memorizzata
                                                            // con caratteri minuscoli.


// CONTROLLA DIZIO
// Alloca la memoria per la nuova parola (solo se e` il primo inserimento in lista
// o se la parola non e` gia` presente); verifica appunto la collocazione nella
// lista: valuta se la testa e` o meno = NULL, a che punto la parola viene
// ad essere ordinata alfabeticamente e a cosa punta la struttura in quel punto
// Esegue la scelta dell'utente (correzione, aggiornamento, prosecuzione, uscita).

lista *Controlla_Dizio( lista * testa, char * vocabolo,
                             char * bigBuffer, char *p)
{
    char c,*t;
    lista *attuale, *nuovo;

    attuale=testa;
    if (testa==NULL){
       if ((nuovo=(lista *) malloc(sizeof (lista)))==NULL){
          printf("2 Impossibile continuare per mancanza di memoria");
          gets(t);
          exit(3);
       }
       strcpy(nuovo->parola,vocabolo);
       c=menu(vocabolo);
       do{
          switch(c){
             case '1':
                testa=Correggi(testa,vocabolo,p,bigBuffer);
                break;
             case '2':                            // La situazione si puo` presentare se
                testa=Primo_ins(nuovo, attuale);  // il dizionario non e` ancora
                break;                            // stato riempito da nulla.
             case '3':
                return(testa);
             case '4':
                exit (4);
          }
       }while (c!='1' && c!='2' && c!='3' );
       return(testa);
    }

    while ((strcmp(attuale->parola,vocabolo)<0)&&(attuale->dopo!=NULL)){
       attuale=attuale->dopo;           // Avanza fino alla collocazione ordinata del
    }                                   // vocabolo nella lista e,o fino a fine-lista.

    if ((strcmp(attuale->parola,vocabolo)==0))
       return (testa);     // La parola e` presente: ritorna.
    else {
       if ((nuovo=(lista *) malloc(sizeof (lista)))==NULL){
          printf("3 Impossibile continuare per mancanza di memoria");
          gets(t);
          exit(3);
       }
       strcpy(nuovo->parola,vocabolo);
    }

    do{
       c=menu(vocabolo);
       }while (c!='1' && c!='2' && c!='3' && c!='4');

        if (strcmp(attuale->parola,vocabolo)>0){
           if (attuale->prima!=NULL){
              switch(c){
                 case '1':
                    testa=Correggi( testa,vocabolo,p,bigBuffer);
                    break;
                 case '2':
                    In_mezzo(nuovo,attuale);
                    break;
                 case '3':                            // Inserimento compreso fra
                    return(testa);
                 case '4':                            // la testa e la coda.
                    exit (4);
              }
           }else {
              switch(c){
                 case '1':
                    testa=Correggi( testa,vocabolo,p,bigBuffer );
                    break;
                 case '2':
                    testa=In_testa(nuovo ,attuale);
                    break;                           // La parola si inserisce
                 case '3':
                    return(testa);                   // ad inizio lista.
                 case '4':
                    exit (4);
              }
        }
           }else if ((strcmp(attuale->parola,vocabolo))<0){
              switch(c){
                 case '1':
                    testa=Correggi(testa,p,vocabolo,bigBuffer );
                    break;
                 case '2':
                    In_coda(nuovo,attuale);
                    break;
                 case '3':                         // L'inserimento avviene
                    return(testa);
                 case '4':                         // in coda lista.
                    exit (4);
              }
           } return (testa);
}



lista* In_testa(lista *nuovo, lista * attuale)
{
    nuovo->dopo=attuale;
    nuovo->prima=NULL;
    attuale->prima=nuovo;
    return (nuovo);
}

void In_mezzo(lista * nuovo, lista * attuale)
{
    nuovo->prima=attuale->prima;
    nuovo->dopo=attuale;
    (attuale->prima)->dopo=nuovo;
    attuale->prima=nuovo;
}

void In_coda(lista * nuovo, lista * attuale)
{
    nuovo->dopo=NULL;
    nuovo->prima=attuale;
    attuale->dopo=nuovo;
}

lista* Primo_ins(lista * nuovo, lista * attuale)
{
    nuovo->dopo=NULL;
    nuovo->prima=NULL;
    attuale=nuovo;
    return (attuale);
}





char menu (char * vocabolo){
    char c;

    printf("\nLa parola \" %s \" manca dal dizionario:\n\n    1) CORREGGI\n    2) AGGIORNA",vocabolo);
    printf("\n    3) CONTINUA (senza modifiche)\n    4) ESCI DAL PROGRAMMA");
    printf( "           Inserire la scelta:   ");
    c=getche();
    return(c);       // Acquisisce la scelta dell'utente
}




// Legge la correzione e modifica il Buffer del testo (residente in
// memoria) utilizzando una sua copia temporanea per non perdere il testo
// non ancora controllato; controlla se vi sono spazi che impliceherebbero
// il controllo su piu` parole (tale controllo e` eseguito dalla
// stessa funzione che caricava le parole del dizionario.


lista * Correggi( lista * testa,char * vocabolo,
                       char * p, char * bigBuffer)
{
    int i,j;
    char * pCorr,*t, correzione[LINE_LENGHT],  *localBuffer;

    printf("\nImmetti la correzione: ");
    gets(correzione);

    if ( (localBuffer=(char *)malloc (TEXTMEMORY))==NULL){        // Allocazione
       printf("55 Impossibile continuare per mancanza di memoria");  // temporanea
       gets(t);
       exit(5);
    }
    strcpy(localBuffer,p);
    *p=NULL;                                       // Modifica in memoria
    strcat(bigBuffer,correzione);                  // del testo originale
    for(;*vocabolo!=NULL;*localBuffer++,*vocabolo++);
    strcat(bigBuffer,localBuffer);
    puts(bigBuffer);
    free(localBuffer);

    j=0;
    //i=0;
    pCorr=correzione;
    do{
       while(correzione[j]!=' ' && correzione[j]!=NULL) j++;
       if ( correzione[j]==' ' ){
          i=1;                                          // Finche` non c'e` il terminatore
          correzione[j]=NULL;                           // dividiamo la correzione in
       }else i=0;                                       // tokens delimitati dallo spazio

       testa = Carica_Lista( strlwr(pCorr), testa );

       if (i==1){                                       // pCorr avanza lungo la correzione
          j++;                                          // puntando ai vari tokens.
          pCorr=&correzione[j];
       }
    }while(i==1);

    return testa;
}
