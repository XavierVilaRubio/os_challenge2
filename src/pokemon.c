#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "pokemon.h"
#include <time.h>

int main () {
	srand(time(NULL));
	while(1){
		int num = (rand() % 9) + 1;
		write(1, &num, sizeof(int));
	}
	exit(0);
}