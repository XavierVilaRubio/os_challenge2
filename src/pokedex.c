#include <stdio.h>       /* fopen, fprintf, fread, fscanf, fseek, ftell, fclose */
#include <string.h>      /* strcmp, strncpy */
#include <stdlib.h>      /* malloc, free, exit, atoi */
#include <unistd.h>      /* sleep, unlink, getpid */
#include <sys/wait.h>    /* wait */
#include <signal.h>
#include <errno.h>
#include "pokemon.h"
#include <string.h>

struct pokemon pokedex[151];

int main(int argc, char** argv) {

  FILE* f = fopen("pokedex.csv", "r");

  if(!f) {
    perror("fopen");
    exit(1);
  }

  char *buf = malloc(151);
  char *info;
  int i = 0;

  while (fgets(buf, 151, f) != NULL) {

    if ((strlen(buf)>0) && (buf[strlen (buf) - 1] == '\n'))
        buf[strlen (buf) - 1] = '\0';

    struct pokemon p;

    info = strtok(buf, ",");
    int id  = atoi(info);
    p.id = id;
    info = strtok(NULL, ","); strcpy(p.name, info);
    info = strtok(NULL, ","); strcpy(p.type[0], info);
    info = strtok(NULL, ","); strcpy(p.type[1], info);
    info = strtok(NULL, ","); p.total = atoi(info);
    info = strtok(NULL, ","); p.hp = atoi(info); 
    info = strtok(NULL, ","); p.attack = atoi(info);
    info = strtok(NULL, ","); p.defense = atoi(info);
    info = strtok(NULL, ","); p.spAttack = atoi(info);
    info = strtok(NULL, ","); p.spDefense = atoi(info);
    info = strtok(NULL, ","); p.speed = atoi(info);
    info = strtok(NULL, ","); p.generation = atoi(info);
    info = strtok(NULL, ","); p.legendary = atoi(info);
    pokedex[i]=p;
    i++;
  }
  
  fclose(f);

  // @Jordi: Jo no faria anar SIGCONT en aquest cas millor, SIGUSR1 o SIGUSR2
  // Ja que, si no ho demano SIGCONT ja té les seves funcions per defecte.
  kill(getppid(), SIGCONT);

  //while(1){
  int pokemonId;

    // @Bug: El problema es que pokemonid tenia un nombre més gran que 151 i al buscar al array segmenation fault.
    // scanf("%d", &pokemonId);

    //@Jordi: Una solució utiltizar el read, ja que l'scanf té un comportament diferent.
    //        Mira el pare, on t'he deixat les línies comentades per que et funcioni amb el scanf també.
    read(0,&pokemonId,sizeof(int));

    //@Jordi: Amb aquest xivato pot comprovar el que li envies si actives el scanf, per veure el problema que et comento.
    printf("he rebut... %d\n",pokemonId);fflush(stdout);

    struct pokemon p;
    p = pokedex[pokemonId - 1];

    // @Jordi: Si fas anar printf, assegura't de fer stdout al final, si no pot ser que no observis el resultat.
    printf("The pokemon with id:%d it is known as: %s.\n", pokemonId, p.name);
    fflush(stdout);
 //}

  exit(0);
}
