#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "pokemon.h"

int main ()
{
  struct pokemon pkm;
  read(0, &pkm, sizeof(struct pokemon));
  char s[100];
  sprintf(s, "%s@@@@@@@@@@@@@@@@@@@@@@@@@\n", "\x1B[36m");
  write(1, s, strlen(s));

  sprintf(s, " %s (%d)\n", pkm.name, pkm.id);
  write(1, s, strlen(s));
  sprintf(s, " +++ Type1: %s, Type2: %s\n", pkm.type[0], pkm.type[1]);
  write(1, s, strlen(s));
  sprintf(s, " +++ Total: %d, Hp: %d\n", pkm.total, pkm.hp);
  write(1, s, strlen(s));
  sprintf(s, " +++ Attack: %d, Defense: %d\n", pkm.attack, pkm.defense);
  write(1, s, strlen(s));
  sprintf(s, " +++ SpAttack: %d, SpDefense: %d, Speed: %d\n", pkm.spAttack, pkm.spDefense, pkm.speed);
  write(1, s, strlen(s));
  sprintf(s, " +++ Gen: %d Legendary: %d\n", pkm.generation, pkm.legendary);
  write(1, s, strlen(s));

  sprintf(s, "@@@@@@@@@@@@@@@@@@@@@@@@@%s\n", "\x1B[0m");
  write(1, s, strlen(s));
  kill(getppid(), SIGUSR1);
  while(1){
    int num;
    read(0, &num, sizeof(int));
    if(num==0)
      exit(0);
    else
      kill(getppid(), SIGUSR2);
  }
  exit(0);

}