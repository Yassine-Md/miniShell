#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <signal.h>
#include "readcmd.h"
#include "utils.h"
#include "commandes_internes.h"
#include "csapp.h"

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define NUM_PIPES 2
#define MAX_JOBS 10
#define MAX_COMMAND_LENGTH 100

// Structure pour stocker les informations sur un job
typedef struct {
    pid_t pid;        // PID du processus
    int status;       // Statut du processus (terminer, en cours, etc.)
    char command[MAX_COMMAND_LENGTH];  // Commande du processus
} Job;

char command[100];
Job jobs[MAX_JOBS]; // tableau des jobs
int numJobs = 0; // la derniere commande mise en arriere-plan

int** allocateDescripteurs(int nbPipes) {
    int** fd = malloc(sizeof(int*) * nbPipes);
    if (fd == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Allouer deux cases pour chaque i
    for (int i = 0; i < nbPipes; i++) {
        fd[i] = malloc(sizeof(int) * 2);
        if (fd[i] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
    }

    return fd;
}

void closeAllPipe(int **fd , int nbPipe) {
    for (int j = 0; j < nbPipe; j++) {
        close(fd[j][0]);
        close(fd[j][1]);
    }
}

void redirectIOPipe(int cmdCourant , int nbPipes, int **fd) {
    if (cmdCourant != 0) { // si ce n'est pas la 1ere commande
        if(dup2(fd[cmdCourant-1][0], STDIN_FILENO) == -1){
            perror("dup2 stdin");
            exit(EXIT_FAILURE);
        }
    }

    if (cmdCourant != nbPipes) { // si ce n'est pas la derniere commande
        if(dup2(fd[cmdCourant][1], STDOUT_FILENO) == -1){
            perror("dup2 stdout");
            exit(EXIT_FAILURE);
        }
    }
}

void handlerSigInt(int sig) {
    fprintf(stderr, "Processus fils %d : Signal SIGINT reçu. Terminaison en cours...\n", getpid());
    kill(getpid(), SIGINT);
}

void removeJob(int index) {
    if (index >= 0 && index < numJobs) {
        for (int i = index; i < numJobs - 1; i++) {
            jobs[i] = jobs[i + 1];
        }
        numJobs--;
    }
}

void SigChildHandler(int sig) {
    pid_t pid;
    int status;

    // La boucle while sert e traiter tous les processus fils qui ont change d'etat
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        // Recherche du job correspondant au PID dans le tableau des jobs
        for (int i = 0; i < numJobs; i++) {
            if (jobs[i].pid == pid) {
                jobs[i].status = status;

                // Si le processus fils a ete terminer normalement ou a cause d'un signal
                if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    // Afficher le message indiquant que le job a  ete terminer
                    printf("[%d] Terminé %s\n", i + 1, jobs[i].command);

                    // Supprimer le job du tableau des jobs
                    removeJob(i);
                }
                // Si le processus fils a  ete arreter
                else if (WIFSTOPPED(status)) {
                    // Afficher le message indiquant que le job a ete arreter
                    printf("[%d] Arrêté %s\n", i + 1, jobs[i].command);
                }

                // On a trouve le job correspondant au PID, on peut sortir de la boucle
                break;
            }
        }
    }
}


void addJob(pid_t pid, char* command) {
    if (numJobs < MAX_JOBS) {
        jobs[numJobs].pid = pid;
        jobs[numJobs].status = 0; // Initialiser le statut
        strncpy(jobs[numJobs].command, command, MAX_COMMAND_LENGTH);
        numJobs++;
    } else {
        fprintf(stderr, "Nombre maximal de jobs atteint\n");
    }
}



void printJobs() {
    for (int i = 0; i < numJobs; i++) {
        printf("[%d] %d %s\n", i + 1, jobs[i].pid, jobs[i].command);
    }
}

void handlerCtrlZ(int sig) {
    // Handle Ctrl+Z in the child processes
    printf("Child process %d received Ctrl+Z\n", getpid());
    addJob(getpid(), command);  // Ajouter le processus fils aux jobs
    kill(getpid(), SIGSTOP);     // Stop the child processs
}
void parentIgnoreCtrlZ(int sig) {
    // Ignore the Ctrl+Z signal in the parent process
    printf("Ctrl Z était ignoré dans le parent\n");
}

void pipeCommande(cmdline *l) {
    int n = nbCmd(l);
    int nbPipes = n - 1;
    int numPipe = 0;

    int** fd = allocateDescripteurs(nbPipes);

    pid_t childpid;

    for (int i = 0; l->seq[i] != NULL; i++) {
        if (i < nbPipes) {
            if (pipe(fd[i]) != 0) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
            numPipe++;
        }
        childpid = Fork();
        printf("childpid %d\n",childpid);
        if (childpid == 0) { // fils
            Signal(SIGINT, handlerSigInt);
            Signal(SIGTSTP, handlerCtrlZ);

            if (l->background) {
                printf("Processus %d en cours d'exécution en arrière-plan...\n", getpid());
            }

            redirectIOPipe(i, nbPipes, fd);
            closeAllPipe(fd, numPipe);

            execCmdWithPipe(l, i);
            exit(0);
        }else { // 
            Signal(SIGTSTP, parentIgnoreCtrlZ);
            if (l->background) {
                // Parent process - add the job without waiting
                addJob(childpid, l->seq[i][0]);
            }
            if (i != 0) {
                close(fd[i-1][1]);
            }
            //Waitpid(childpid, NULL, 0);
        }
    }
}

int main() {
    Signal(SIGINT, SIG_IGN);
    Signal(SIGCHLD, SigChildHandler);

    cmdline *l;
    while (1) {
        printf("shell> ");
        l = readcmd();
        affichage(l);

        if (l->seq[0] != NULL) {
            if (strcmp(l->seq[0][0], "jobs") == 0) {
                printJobs();
            } else {
                Signal(SIGTSTP, parentIgnoreCtrlZ);
                pipeCommande(l);
            }
        }
    }

    return 0;
}
