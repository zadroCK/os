#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

void obradi_dogadjaj(int sig);
void obradi_sigterm(int sig);
void obradi_sigint(int sig);

int broj;
int nije_kraj = 1;
FILE *status;
FILE *obradi;



int main()
{

    struct sigaction act;
    /* 1. maskiranje signala SIGUSR1 */
    act.sa_handler = obradi_dogadjaj; /* kojom se funkcijom signal obrađuje */
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGTERM); /* blokirati i SIGTERM za vrijeme obrade */
    act.sa_flags = 0; /* naprednije mogućnosti preskočene */
    sigaction(SIGUSR1, &act, NULL); /* maskiranje signala preko sučelja OS-a */
    /* 2. maskiranje signala SIGTERM */
    act.sa_handler = obradi_sigterm;
    sigemptyset(&act.sa_mask);
    sigaction(SIGTERM, &act, NULL);
    /* 3. maskiranje signala SIGINT */
    act.sa_handler = obradi_sigint;
    sigaction(SIGINT, &act, NULL);

    status = fopen("status.txt","r");
    if(status == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }
        
    //procitaj broj
    fscanf(status,"%d", &broj);

    fclose(status);
    if(broj == 0)
    {
        obradi  = fopen("obradi.txt","r");
        int num;
        while(!feof (obradi))
        {
            fscanf(obradi, "%d", &num);
        }
        broj = (int)sqrt((double)num);
        fclose(obradi);
    }

    status = fopen("status.txt","w");
    fprintf(status, "%d", 0);
    fclose(status);

    printf("Program s PID=%ld krenuo s radom\n", (long) getpid());
    int i = 1;
    while(nije_kraj) {
        //printf("Broj koji trenurno obradujem: %d\n", broj);
        broj = broj + 1;
        int x = broj*broj;
        obradi  = fopen("obradi.txt","a");
        fprintf(obradi, " %d ", x);
        fclose(obradi);

        sleep(5);
    }


    return 0;
}


void obradi_dogadjaj(int sig)
{
    printf("Broj: %d\n", broj);
}
void obradi_sigterm(int sig)
{
    status = fopen("status.txt","w");
    fprintf(status, "%d", broj);
    fclose(status);
    exit(1);
}
void obradi_sigint(int sig)
{
    exit(1);
}