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

#define TRUE 1
#define FALSE 0
#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"
enum State
{
    Ready = 1,
    NotReady = 0,
    Winner = 2,
    Looser = 3,
    Attack = 4,
    Defense = 5
};
enum State currentStatus;

void handlerSIGUSR2()
{
    if (currentStatus > 3)
        currentStatus = Winner;
    else
        currentStatus++;
}

void handlerSIGUSR1()
{
    if (currentStatus == Attack)
        currentStatus = Defense;
    else if (currentStatus == Defense)
        currentStatus = Attack;
}

char *rivalColour;
char *myColour;

int genRand(int num)
{
    return (rand() % num) + 1;
}

int main(int arc, char *arv[])
{
    srand(getpid());
    currentStatus = NotReady;
    signal(SIGUSR1, handlerSIGUSR1);
    signal(SIGUSR2, handlerSIGUSR2);
    char s[200];
    //Generate a random integer between 1 and 151.
    int id = genRand(151);
    //Ask information about this Pokemon to the Pokedex process.
    write(4, &id, sizeof(int));
    struct pokemon pkm;
    read(0, &pkm, sizeof(struct pokemon));
    int original_hp = pkm.hp;

    //Tell Ash (parent) that it is ready to fight.
    kill(getppid(), SIGUSR1);
    //read(0, myColour, sizeof(char));
    //read(0, rivalColour, sizeof(char));

    //Wait to obtain information about their configuration (file descriptor, colours,etc.).
    int fd[2];
    if (atoi(arv[1]) == 0)
    {
        fd[0] = 7;
        fd[1] = 10;
        myColour = KCYN;
        rivalColour = KMAG;
    }
    else
    {
        fd[0] = 9;
        fd[1] = 8;
        myColour = KMAG;
        rivalColour = KCYN;
    }

    while (currentStatus == NotReady)
    {
    }
    currentStatus = (atoi(arv[1])) ? Attack : Defense;
    //Welcoming the Pokemon rival exchanging information ( PID and pokemon).
    pid_t pid = getpid();
    write(fd[1], &pid, sizeof(pid_t));
    pid_t rivalPid;
    read(fd[0], &rivalPid, sizeof(rivalPid));
    write(fd[1], &pkm, sizeof(struct pokemon));
    struct pokemon rivalPkm;
    read(fd[0], &rivalPkm, sizeof(rivalPkm));

    srand(time(NULL));
    while (currentStatus > 3)
    {
        int dmg = genRand(10);
        sprintf(s, "%s[%d] ########%s [%d/%d]%s\n", myColour, getpid(), pkm.name, pkm.hp, original_hp, KNRM);
        write(1, s, strlen(s));
        sleep(1);
        if (currentStatus == Attack)
        {
            sprintf(s, "%s[%d] ########%s [%d/%d]%s ---(Send attack with damage(%d))---> %s%s[%d]%s\n", myColour, getpid(), pkm.name, pkm.hp, original_hp, KGRN, dmg, rivalColour, rivalPkm.name, rivalPid, KNRM);
            write(1, s, strlen(s));
            write(fd[1], &dmg, sizeof(int));
            kill(rivalPid, SIGUSR1);
        }
        else if (currentStatus == Defense)
        {
            read(fd[0], &dmg, sizeof(int));
            sprintf(s, "%s[%d] ########%s [%d/%d]%s <---(Send attack with damage(%d))--- %s%s[%d]%s\n", myColour, getpid(), pkm.name, pkm.hp, original_hp, KRED, dmg, rivalColour, rivalPkm.name, rivalPid, KNRM);
            write(1, s, strlen(s));
            pkm.hp -= dmg;
            kill(rivalPid, SIGUSR1);
        }
        if (pkm.hp <= 0)
        {
            pkm.hp = 0;
            kill(rivalPid, SIGUSR2);
            sprintf(s, "%s%s [%d] I have been defeated by %s%s[%d]%s\n", myColour, pkm.name, getpid(), rivalColour, rivalPkm.name, rivalPid, KNRM);
            write(1, s, strlen(s));
            sprintf(s, "%s[%d] ########%s [%d/%d]%s\n", myColour, getpid(), pkm.name, pkm.hp, original_hp, KNRM);
            write(1, s, strlen(s));
            currentStatus = Looser;
        }
        sleep(1);
    }

    exit(0);
}