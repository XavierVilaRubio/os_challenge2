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

pid_t child_pid;
int status;
int fd[2], fd2[2];

void handlerSIG1(){
    currentStatus=WaitingPokemon;
}

void handlerSIGINT(){
    close(fd[1]);
    close(fd2[0]);
    printf("\nTurning off Pokedex...\n");fflush(stdout);
    kill(child_pid, SIGUSR1);
    printf("Pokedex turned off\n");fflush(stdout);
    wait(&status);
    exit(0);
}

int main(int arc, char *arv[])
{
    //Preparem el tractament de senyals
    signal(SIGUSR1, handlerSIG1);
    signal(SIGINT, handlerSIGINT);

    //Creem les pipes
    pipe(fd);
    pipe(fd2);
    //Guardem el pid del fill per a poder-lo matar
    child_pid = fork();
    if (child_pid == 0)
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
    while(currentStatus==WaitingPokedex){}
    printf("[%d] Pokedex is ready to search info...\n", getpid());
    fflush(stdout);

    //Tanquem descriptors innecessaris
    close(fd[0]);
    close(fd2[1]);

    while (1)
    {
        //Resetejem la variable per a que entri al while
        int pokemonId = 0;
        while (pokemonId < 1 || pokemonId > 151)
        {
            printf("\nEnter a pokemonId between (1-151): ");
            scanf("%d", &pokemonId);
        }

        //Li passem el pokemonId introduit a la pokedex per la pipe(fd)
        write(fd[1], &pokemonId, sizeof(int));

        struct pokemon pkm;
        //Llegim l'apuntador de l'estructura que ens ha passat la pokedex per la pipe(fd2)
        read(fd2[0], &pkm, sizeof(struct pokemon));
        //Prints per comprovar que hem rebut l'apuntador de manera correcta
        printf("Pokemon name: %s\n", pkm.name);
        printf("Pokemon hp: %d\n", pkm.hp);
    }

    exit(0);
}
