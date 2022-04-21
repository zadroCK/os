#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "signali.h"
#include "datoteke.h"
#define BROJ_DRETVI 3

char *dat_status, *dat_obrada, *dat_mreza;
int broj = 0;
int nije_kraj = 1;

/* funkcije koje rade dretve */
void *obrada(void *);
void *mreza(void *);

int main(int argc, char *argv[])
{
	if (argc < 4) {
		fprintf(stderr, "Koristenje: %s <status-datoteka> "
		 "<datoteka-s-podacima> <cjevovod>\n", argv[0]);
		exit(1);
	}
    pthread_t opisnik[BROJ_DRETVI];
    int id[BROJ_DRETVI];

	dat_status = argv[1];
	dat_obrada = argv[2];
	dat_mreza = argv[3];

	postavi_signale();
	/* dohvati prvi broj i zapisi 0 u status */
    broj = procitaj_status();
    zapisi_status(0);
	/* todo */

	/* stvori dretve: radnu i mreznu */
    if (pthread_create(&opisnik[0], NULL, obrada, &id[0]))
    {
        fprintf(stderr, "Ne mogu stvoriti novu dretvu!\n");
        exit(1);
    }
    if (pthread_create(&opisnik[0], NULL, mreza, &id[1]))
    {
        fprintf(stderr, "Ne mogu stvoriti novu dretvu!\n");
        exit(1);
    }
	/* todo */

	while(nije_kraj) {
		//procitaj broj iz konzole, npr. sa scanf
		//ako je > 0 onda ga postavi kao broj
		//inace prekidni s radom => postavi nije_kraj = 0
        int num;
        scanf("%d", &num);
        if(num > 0)
            broj = num;
        else
            nije_kraj = 0; 
	}

	//cekaj na kraj dretve obrada, ali ne i one druge mrezne
    pthread_join(opisnik[0], NULL);

	return 0;
}

void *obrada(void *p)
{
    printf("Program s PID=%ld krenuo s radom\n", (long) getpid());
    while(1) {
        printf("Broj koji trenurno obradujem: %d\n", broj);
        broj = broj + 1;
        int x = broj*broj;
        dodaj_broj(x);

        sleep(5);
    }
}

void *mreza(void *p)
{
	//u petlji cekaj da se nesto pojavi u cijevi
	//ako je procitano > 0 onda ga postavi u broj
    while(1)
    {
        int num = dohvati_iz_cijevi();
        if(num > 0)
            broj = num;
    }

	return NULL;
}