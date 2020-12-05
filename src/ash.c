#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "packet.h"
#include "pokemon.h"
#include <time.h>

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

char *args[] = {"pokemon", "pokemon", NULL};
char *args2[] = {"pokedex", "pokedex", NULL};

#define TRUE 1
#define FALSE 0
enum State
{
	WaitingPokedex = 1,
	WaitingPokemon = 2,
	Fighting = 3,
	EndFight = 4
};
enum State currentStatus;

pid_t pokemon_pid;
int status;
int fd[2], fd2[2], fd3[2];
int endFlag=1;
char s[100];
int num;

void handlerSIGUSR1(){
	currentStatus++;
}

//Processa les accions dels pokemon
void fightPokemon(int num){
	switch(num){
		case 2:
			printf("%sGotcha!The pokemon was caught.%s\n", KGRN, KNRM);fflush(stdout);
			kill(pokemon_pid, SIGKILL);
			currentStatus=EndFight;
			break;
		case 7:
			printf("%sThe pokemon escaped already.%s\n", KYEL, KNRM);fflush(stdout);
			kill(pokemon_pid, SIGKILL);
			currentStatus=EndFight;
			break;
		default:
			printf("%sOh no!The pokemon broke free.%s\n", KBLU, KNRM);fflush(stdout);
			break;
	}
}

void handlerSIGINT(){
	close(fd[1]);
	close(fd2[0]);
	close(fd3[1]);
	kill(pokemon_pid, SIGKILL);
	wait(&status);
	exit(0);
}

void printPokemon(struct pokemon pkm){
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
}

int main(int arc, char *arv[])
{
	//Preparem el tractament de senyals
	signal(SIGUSR1, handlerSIGUSR1);
	signal(SIGINT, handlerSIGINT);
	srand(time(NULL));

	while (endFlag == 1) { 

		char choice;
		
		sprintf(s, "################\n# E. Explore \n# Q. Quit\n################\n");
		if (write(1, s, strlen(s)) < 0) perror("Error writting the menu");
		scanf(" %c", &choice);
		switch (choice) { 
			case 'Q':
				endFlag=0;
				break; 
			case 'E':
				//Creem les pipes i el fill per interactuar amb la pokedex
				pipe(fd);
				pipe(fd2);
				if (fork() == 0)
				{
					close(0);
					dup(fd[0]);
					close(1);
					dup(fd2[1]);
					close(fd[0]);
					close(fd[1]);
					close(fd2[0]);
					close(fd2[1]);
					//Executem la pokedex
					execv(args2[0], args2);
					exit(0);
				}
				//Esperem a que la pokedex carregui
				currentStatus = WaitingPokedex;
				while (currentStatus == WaitingPokedex){}

				//Tanquem descriptors innecessaris
				close(fd[0]);
				close(fd2[1]);

				//Generem el pokemon aleatori
				int pokemonId = (rand() % 151) + 1;

				//Li passem el pokemonId generat aleatoriament a la pokedex per la pipe(fd)
				write(fd[1], &pokemonId, sizeof(int));

				struct pokemon pkm;
				//Guardem el pokemon que ens retorna la pokedex per la fd2[0]
				read(fd2[0], &pkm, sizeof(struct pokemon));

				//Creem la pipe i el fill per interactuar amb el pokemon
				pipe(fd3);
				pokemon_pid = fork();
				if (pokemon_pid == 0) {
					close(1);
					dup(fd3[1]);
					close(fd[0]);
					close(fd[1]);
					close(fd2[0]);
					close(fd2[1]);
					close(fd3[0]);
					close(fd3[1]);
					//Executem la pokedex
					execv(args[0], args);
					exit(0);
				}
				close(fd3[1]);

				sprintf(s, "Ash:[%d] --> %sWild pokemon appeared [%d]%s\n", getpid(), KBLU, pokemon_pid, KNRM);
				write(1, s, strlen(s));
				//currentStatus: WaitingPokemon --> Fighting
				currentStatus++;
				printPokemon(pkm);
				close(fd[1]);
				close(fd2[0]);
				break; 
			default: 
				sprintf(s, "%s!!!!Invalid option. Try again. %s\n", KRED, KNRM);
				if (write(1, s, strlen(s)) < 0) perror("Error writting invalid option");
				break;
		}
		while (currentStatus == Fighting)
		{
			sprintf(s, "# P. Throw Pokeball \n# R. Run\n");
			if (write(1, s, strlen(s)) < 0) perror("Error writting the menu");
			scanf(" %c", &choice);
			switch (choice)
			{
			case 'P':
				//Llegim el numero aleatori generat pel pokemon
				read(fd3[0], &num, sizeof(int));
				fightPokemon(num);
				break;
			case 'R':
				kill(pokemon_pid, SIGKILL);
				sprintf(s, "Ash [%d] %sused escape rope%s\n", getpid(), KBLU, KNRM);
				write(1, s, strlen(s));
				currentStatus++;
				break;
			default:
				sprintf(s, "%s!!!!Invalid option. Try again. %s\n", KRED, KNRM);
				if (write(1, s, strlen(s)) < 0) perror("Error writting invalid option");
				break;
			}
		}
		close(fd[1]);
		close(fd2[0]);
		close(fd3[1]);
	}

	sprintf(s, "%s!!!!I'm tired from all the fun... %s\n", KMAG, KNRM);
	if (write(1, s, strlen(s)) < 0) perror("Error writting the ending msg");
	exit(0);
}
