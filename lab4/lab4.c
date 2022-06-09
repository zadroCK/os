#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datoteke.h"

#define TEXT	80

int main() {
	int fd;
	char ime[IME_DULJINA], *posn;
	char naredba[10];
	char tekst[80];
	int koliko_puta, i;
	inicijaliziraj_prazni_disk();
	while(1) {
		printf("\nNova naredba:\nUnesi ime datoteke ili 'kraj' za kraj:\n");

		fgets(ime, IME_DULJINA, stdin);
		posn = strchr(ime, '\n');
		if (posn)
			*posn = '\0';
		if (strcmp(ime, "kraj") == 0)
			exit(0);

		printf("Zelis otvoriti datoteku %s za citanje (c) ili pisanje (p)?\n", ime);
		fgets(naredba, 10, stdin);
		if (naredba[0] == 'c') {
			fd = otvori_datoteku(ime, ZA_CITANJE);
			if (fd == -1)
				continue; //poruka je vec ispisana
			/* citaj dio datoteke i ispisi ga, dok procitaj vrati nesto */
			while(procitaj(fd, tekst, BLOCK_SIZE*BLOCKS)> 0);
			printf("%s\n", tekst);
			memset(tekst, 0, 80);
		}
		else if (naredba[0] == 'p') {
			fd = otvori_datoteku(ime, ZA_PISANJE);
			if (fd == -1)
				continue; //poruka je vec ispisana

			printf("Unesi tekst za upisivanje u datoteku (do %d znakova)\n", TEXT);
			fgets(tekst, TEXT, stdin);
			posn = strchr(tekst, '\n');
			if (posn)
				*posn = '\0';
			printf("Koliko puta da ga upisem u datoteku?\n");
			fgets(naredba, TEXT, stdin);
			sscanf(naredba, "%d", &koliko_puta);
			for (i = 0; i < koliko_puta; i++)
				zapisi(fd, tekst, strlen(tekst));
		}
		else {
			fprintf(stderr, "Kriva naredba\n");
		}
		zatvori_datoteku(fd);
	}
}