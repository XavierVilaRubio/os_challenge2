#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "packet.h"
#include "pokemon.h"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

char *args2[] = {"pokedex", "pokedex", NULL};

#define TRUE 1
#define FALSE 0
enum State {WaitingPokedex=1, WaitingPokemon=2, Fighting=3, EndFight=4}; 
enum State currentStatus;

char msg[100];
void cont(){}

int main(int arc, char *arv[])
{
    signal(SIGCONT, cont);
    int status;
    int fd[2], fd2[2];
    int pokemonId;

    pipe(fd);
    if(fork()==0){
        close(0);
        dup(fd[0]);
        close(fd[0]);
        close(fd[1]);
        execv(args2[0], args2);
        exit(0);
    }

    pause();

    //@Jordi: El pare no cal que tanqui la sortida 1, pot fer servir directament el pipe fd[1]
    //close(1);
    //dup(fd[1]);

    close(fd[0]);
    //@Jordi: La neteja de la pipe s'ha de fer quan ja no tinguis que fer-la servir.
    //close(fd[1]);

    //while(1){

    printf("Enter a pokemonId:  ");
    scanf("%d", &pokemonId);

    //@Jordi: El pare en aquest punt pot fer anar directament la pipe
    //@Jordi: Recorda que el write necessita l'@ -> &pokemonId en lloc de pokemonId declarat com int pokemonId.

    //@Jordi: Si li passes així millor i més simple fer un read a pokedex.c.
    //@Jordi: Millor sempre fer anar write i read amb pipes.
    write(fd[1], &pokemonId, sizeof(int));

    //@Jordi: Si vols fer anar scanf a pokedex.c has de passar-li així si no et donarà problemes.
    //char pokemonIdstr[30];
    //sprintf(pokemonIdstr,"%d",pokemonId);
    //write(fd[1],&pokemonIdstr,sizeof(pokemonIdstr));

    // @Jordi: El pare no cal tancar la stdout (1).
    //close(1);
    //}

    wait(&status);

    //@Jordi: Aqui el pare ja no escriura més a la pipe ja que el fill ha acabat, per tant es pot tancar.
    close(fd[1]);

    exit(0);
}
