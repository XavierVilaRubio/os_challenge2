#include <stdio.h>       /* fopen, fprintf, fread, fscanf, fseek, ftell, fclose */
#include <string.h>      /* strcmp, strncpy */
#include <stdlib.h>      /* malloc, free, exit, atoi */
#include <unistd.h>      /* sleep, unlink, getpid */
#include <sys/wait.h>    /* wait */
#include <signal.h>
#include <errno.h>
#include "pokemon.h"

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
  
  // This part must be changed properly
  int pokemonId;
  printf ("Enter a pokemonId between (1-151): ");
  scanf ("%d",&pokemonId);
  struct pokemon p;
  p = pokedex[pokemonId - 1]; 
  printf ("The pokemon with id:%d it is known as: %s.\n",pokemonId,p.name);

  exit(0);
}
