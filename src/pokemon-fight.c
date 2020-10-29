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
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
enum State {Ready=1, NotReady=0, Winner=2, Looser=3, Attack=4,Defense=5}; 
enum State currentStatus;

char * rivalColour;
char * myColour;

int main(int arc, char *arv[])
{

}