#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "datoteke.h"

/* implementacija funkcija, vec opisanih u lab1 + dohvati_iz_cijevi */


FILE *status;
FILE *obradi;

int procitaj_status() 
{ 
    status = fopen(dat_status,"r");
    if(status == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }
        
    //procitaj broj
    int num;
    fscanf(status,"%d", &num);    
    return num; 
}
void zapisi_status(int broj) 
{
    status = fopen(dat_status,"w");
    fprintf(status, "%d", broj);
    fclose(status);
}
void dodaj_broj(int x) 
{
    obradi  = fopen(dat_obrada,"a");
    fprintf(obradi, " %d ", x);
    fclose(obradi);
}

int pronadji_zadnji_broj() 
{ 
    obradi  = fopen(dat_obrada,"r");
    int num;
    while(!feof (obradi))
    {
        fscanf(obradi, "%d", &num);
    }
    
    fclose(obradi);
    return (int)sqrt((double)num); 
}

int dohvati_iz_cijevi()
{
	FILE *fp;
	int x, y = -1;

	fp = fopen(dat_mreza, "r");
	if (!fp) {
		fprintf(stderr, "Ne mogu otvoriti cijev %s\n", dat_mreza);
		exit(1);
	}
	if (fscanf(fp, "%d", &x) > 0)
		y = x;
	fclose(fp);

	return y;
}
