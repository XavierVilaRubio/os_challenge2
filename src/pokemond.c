#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <time.h>

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

#define RUNNING 1

int status = 0;

#define RUNNING_DIR "/home/xavi/tmp" // Heu de crear una carpeta tmp en el directori del vostre usuari i modificar el {user}
#define LOCK_FILE "pokemond.lock"
#define LOG_FILE "pokemond.log"

void log_message(filename, message) char *filename;
char *message;
{
    FILE *logfile;
    time_t curtime;
    //Guarda la data a curtime
    time(&curtime);

    //Converteix la data (tm structure) a string
    char *t = ctime(&curtime);
    //Elimina el salt de linea
    if (t[strlen(t) - 1] == '\n')
        t[strlen(t) - 1] = '\0';

    //Obre el filename i li escriu la data + el message
    logfile = fopen(filename, "a");
    if (!logfile)
        return;
    fprintf(logfile, "[%s] %s \n", t, message);
    fclose(logfile);
}

void signal_handler(sig) int sig;
{
    switch (sig)
    {
    case SIGTERM:
        log_message(LOG_FILE, "Rebuda la señal per aturar el dimoni pokemon \n");
        status = 0;
    }
}

static void daemonize()
{
    pid_t pid;
    int lfp;
    char str[10];

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    signal(SIGCHLD, SIG_IGN); /* ignore child */
    signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGHUP, SIG_IGN);         /* catch hangup signal */
    signal(SIGTERM, signal_handler); /* catch kill signal */

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir(RUNNING_DIR);

    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
    {
        close(x);
    }

    lfp = open(LOCK_FILE, O_RDWR | O_CREAT, 0640);
    if (lfp < 0)
        exit(1); /* can not open */
    if (lockf(lfp, F_TLOCK, 0) < 0)
        exit(0); /* can not lock */
    /* first instance continues */
    sprintf(str, "%d\n", getpid());
    write(lfp, str, strlen(str)); /* record pid to lockfile */

    status = RUNNING;
}

int main()
{
    daemonize();

    while (status == RUNNING)
    {
        char *msg = KNRM "El pokemon està " KBLU " arrancat " KNRM " \n";
        log_message(LOG_FILE, msg);
        sleep(60);
    }

    char *msg = KNRM "El pokemon s'ha " KRED " aturat " KNRM " \n";
    log_message(LOG_FILE, msg);

    return EXIT_SUCCESS;
}