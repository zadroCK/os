#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "signali.h"
#include "datoteke.h"


char *dat_status, *dat_obrada, *dat_mreza;
int broj = 0;
int nije_kraj = 1;

int radi; //0-kad niti jedna dretva ne radi, id-dretve kad ona radi
int broj_dretvi = 1;
int postavljeni_broj_dretvi = 1;
pthread_mutex_t m;
pthread_cond_t red;
pthread_t opisnik[9];
int id[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

//koje dretve su slobodne
//prve tri na pocetku nece biti slobodne jer su redom: obrada, mreza, (nova upravljacka)
int slobodna_dretva[]= {1, 1, 1, 0, 0, 0, 0, 0, 0};


void udi_u_monitor()
{
    pthread_mutex_lock (&m);
    while(radi)
        pthread_cond_wait (&red, &m);
    radi = 1;
    pthread_mutex_unlock (&m);
}

void izadi_iz_monitora()
{
    pthread_mutex_lock (&m);
    radi = 0;
    pthread_mutex_unlock (&m);
    pthread_cond_broadcast (&red);
}

/* funkcije koje rade dretve */
void *obrada(void *);
void *mreza(void *);
void *upravljacka(void *);

int main(int argc, char *argv[])
{
	if (argc < 4) {
		fprintf(stderr, "Koristenje: %s <status-datoteka> "
		 "<datoteka-s-podacima> <cjevovod>\n", argv[0]);
		exit(1);
	}

    pthread_mutex_init (&m, NULL);
    pthread_cond_init (&red, NULL);
    radi = 0;



	dat_status = argv[1];
	dat_obrada = argv[2];
	dat_mreza = argv[3];

	postavi_signale();


	/* dohvati prvi broj i zapisi 0 u status */
    broj = procitaj_status();
    zapisi_status(0);
	
    
	/*stvori dretve: radnu, mreznu i novu upravljacku */
    if (pthread_create(&opisnik[0], NULL, obrada, &id[0]))
    {
        fprintf(stderr, "Ne mogu stvoriti novu dretvu!\n");
        exit(1);
    }
    if (pthread_create(&opisnik[1], NULL, mreza, &id[1]))
    {
        fprintf(stderr, "Ne mogu stvoriti novu dretvu!\n");
        exit(1);
    }
    if (pthread_create(&opisnik[2], NULL, upravljacka, &id[2]))
    {
        fprintf(stderr, "Ne mogu stvoriti novu dretvu!\n");
        exit(1);
    }
	

	while(nije_kraj) {
        udi_u_monitor();
		//procitaj broj iz konzole, npr. sa scanf
		//ako je > 0 onda ga postavi kao broj
		//inace prekidni s radom => postavi nije_kraj = 0
        printf("Unesi broj: \n");
        int num;
        scanf("%d", &num);
        if(num > 0)
            broj = num;
        else
            nije_kraj = 0;
        izadi_iz_monitora(); 
        sleep(3);
	}

	//cekaj na kraj dretve obrada, ali ne i one druge mrezne
    pthread_join(opisnik[0], NULL);


    pthread_mutex_destroy (&m);
    pthread_cond_destroy (&red);

	return 0;
}

void *obrada(void *p)
{
    printf("Program s PID=%ld krenuo s radom\n", (long) getpid());
    while(1) {
        if(broj_dretvi > postavljeni_broj_dretvi)
        {
            int id = *((int *) p);
            slobodna_dretva[id] = 0;
            broj_dretvi--;
            pthread_exit(NULL);
        }
        udi_u_monitor();
        broj = broj + 1;
        int x = broj*broj;
        dodaj_broj(x);

        izadi_iz_monitora();

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
        udi_u_monitor();
        if(num > 0)
            broj = num;
        izadi_iz_monitora();
    }

	return NULL;
}


void *upravljacka(void *p)
{
    while(1)
    {
        udi_u_monitor();
        printf("Postavi broj radnih dretvi: \n");
        int num;
        scanf("%d", &num);
        postavljeni_broj_dretvi = num;
        if(postavljeni_broj_dretvi < 8)
        {
            if(postavljeni_broj_dretvi > broj_dretvi)
            {
                int i = 0;
                for(i = 0; i < postavljeni_broj_dretvi - broj_dretvi; i++)
                {
                    int j = 0;
                    for(j = 0; j < 10; j++)
                    {
                        if(slobodna_dretva[j] == 0)
                        {
                            if (pthread_create(&opisnik[j], NULL, obrada, &id[j]))
                            {
                                fprintf(stderr, "Ne mogu stvoriti novu dretvu!\n");
                                exit(1);
                            }
                            break;
                        }
                    }
                }
                broj_dretvi = postavljeni_broj_dretvi;
            }

        }
        else
            printf("Maksimalni broj radnih dretvi je 7\n");
        
        izadi_iz_monitora();
        sleep(5);
    }
}