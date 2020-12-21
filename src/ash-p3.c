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
char *args3[] = {"pokemon-fight", "0", NULL};
char *args4[] = {"pokemon-fight", "1", NULL};

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
int fd[2], fd2[2], fd3[2], fd4[2], pkmns = 0;
char s[100];

void handlerSIGUSR1()
{
	currentStatus++;
}

int genRand(int num)
{
	srand(getpid());
	return (rand() % num);
}

int checkDeamon()
{
	char line[6];
	FILE *cmd = popen("pidof -x pokemond", "r");

	fgets(line, 6, cmd);
	pid_t pid = strtoul(line, NULL, 10);

	pclose(cmd);
	if (pid == 0)
	{
		sprintf(s, "%s[ERROR] To play a pokemon game, the pokemon daemon must be started and now is stopped.%s\n", KBLU, KNRM);
		write(2, s, strlen(s));
		exit(2);
	}
	else
	{
		return 1;
	}
}

int main(int arc, char *arv[])
{
	checkDeamon();
	int status;
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
		exit(0);
	}
	//ash is waiting pokedex to be ready
	currentStatus = WaitingPokedex;
	while (currentStatus == WaitingPokedex)
	{
	}
	sprintf(s, "[%d] Pokedex is ready to search info...\n", getpid());
	write(1, s, strlen(s));

	//ash creates two child pokemon_fight.c
	pipe(fd3);
	pipe(fd4);
	pokemon1_pid = fork();
	if (pokemon1_pid == 0)
	{
		close(0);
		dup(fd2[0]);
		close(fd[0]);
		close(fd2[0]);
		close(fd2[1]);
		execv(args3[0], args3);
		close(fd[1]);
		close(fd3[0]);
		close(fd3[1]);
		close(fd4[0]);
		close(fd4[1]);
		exit(0);
	}
	pokemon2_pid = fork();
	if (pokemon2_pid == 0)
	{
		close(0);
		dup(fd2[0]);
		close(fd[0]);
		close(fd2[0]);
		close(fd2[1]);
		execv(args4[0], args4);
		close(fd[1]);
		close(fd3[0]);
		close(fd3[1]);
		close(fd4[0]);
		close(fd4[1]);
		exit(0);
	}
	//ash waits the child to be ready
	currentStatus = WaitingPokemon;
	while (currentStatus == WaitingPokemon)
	{
	}
	currentStatus = WaitingPokemon;
	while (currentStatus == WaitingPokemon)
	{
	}
	//info
	sprintf(s, "[%d] Pokedemons are ready to fight...\n", getpid());
	write(1, s, strlen(s));
	kill(pokemon1_pid, SIGUSR2);
	kill(pokemon2_pid, SIGUSR2);

	sprintf(s, "[%d] Fight starts...\n", getpid());
	write(1, s, strlen(s));

	close(fd[0]);
	close(fd[1]);
	close(fd2[0]);
	close(fd2[1]);
	close(fd3[0]);
	close(fd3[1]);
	close(fd4[0]);
	close(fd4[1]);
	//wait pokemons end
	wait(&status);
	wait(&status);
	sprintf(s, "[%d] The fight ends...\n", getpid());
	write(1, s, strlen(s));

	//kill and wait pokedex and end
	kill(pokedex_pid, SIGKILL);
	wait(&status);

	exit(0);
}