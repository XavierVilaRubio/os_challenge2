#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "packet.h"
#include "pokemon.h"

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

char *args2[] = {"pokedex", "pokedex", NULL};
char *args3[] = {"pokemon-fight", "pokemon-fight", NULL};

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

pid_t pokedex_pid, pokemon1_pid, pokemon2_pid;
int fd[2], fd2[2], fd3[2];
char s[100];

void handlerSIGUSR1()
{
	currentStatus++;
}

int main(int arc, char *arv[])
{
	signal(SIGUSR1, handlerSIGUSR1);

	pipe(fd);
	pipe(fd2);
	pokedex_pid = fork();
	if (pokedex_pid == 0)
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
		//exit(0);
	}
	//1.ash is waiting pokedex to be ready
	currentStatus = WaitingPokedex;
	while (currentStatus == WaitingPokedex)
	{
	}
	sprintf(s, "[%d] Pokedex is ready to search info...\n", getpid());
	write(1, s, strlen(s));

	//2.ash creates two child pokemon_fight.c
	pokemon1_pid = fork();
	if (pokemon1_pid == 0)
	{
		close(0);
		dup(fd2[0]);
		close(1);
		dup(fd[1]);
		close(fd[0]);
		close(fd[1]);
		close(fd2[0]);
		close(fd2[1]);
		execv(args3[0], args3);
		//exit(0);
	}
	//3.ash waits the child to be ready
	currentStatus = WaitingPokemon;
	while (currentStatus == WaitingPokemon)
	{
	}
	sprintf(s, "[%d] Pokedemon1 is ready to fight...\n", getpid());
	write(1, s, strlen(s));

	/*
	pokemon2_pid = fork();
	if (pokemon2_pid == 0)
	{
		close(0);
		dup(fd2[0]);
		close(1);
		dup(fd[1]);
		close(fd[0]);
		close(fd[1]);
		close(fd2[0]);
		close(fd2[1]);
		execv(args3[0], args3);
	}
	*/

	//4.info
	//5.end fight
	//6.wait pokemons end
	//7.kill and wait pokedex and end
	wait(NULL);
	wait(NULL);

	exit(0);
}