#include "pipe.h"

extern pid_t childpid;
volatile int foregroundProcessCompleted ;
//extern Job jobs[MAX_JOBS];

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


void Pipe(int *fd) {
    if (pipe(fd) != 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
}

// Fonction pour attendre le processus en avant-plan
void waitForForegroundProcess(int background) {
    while (!foregroundProcessCompleted && (background == 0)) {
        sleep(1);
    }
}

// Fonction pour lancer un processus fils
void launchChild(cmdline *l, int i, int numPipe, int nbPipes, int **fd) {
    childpid = Fork();
    printf("childpid %d\n", childpid);

    if (childpid == 0) { // Fils
        Signal(SIGINT, handlerSigInt);
        // Ajouter cette ligne pour retablir le gestionnaire par defaut pour SIGCHLD
        Signal(SIGCHLD, SIG_DFL);

        if (l->background) {
            printf("Processus %d en cours d'exécution en arrière-plan...\n", getpid());
        }

        redirectIOPipe(i, nbPipes, fd);
        closeAllPipe(fd, numPipe);

        execCmdWithPipe(l, i);
        exit(0);
    }
}

void pipeCommande(cmdline *l) {
    int n = nbCmd(l);
    int nbPipes = n - 1;
    int numPipe = 0;

    int** fd = allocateDescripteurs(nbPipes);

    // Initialiser la variable globale à 0
    foregroundProcessCompleted = 0;

    for (int i = 0; l->seq[i] != NULL; i++) {
        if (i < nbPipes) {
            pipe(*fd);
            numPipe++;
        }
        launchChild(l, i, numPipe, nbPipes, fd);

    // pere
        // Parent process - add the job without waiting
        if(l->background){
            addJob(childpid, l->seq[i][0]);
        }

        if(!l->background && i == n-1){
            // Si c'est la derniere commande et qu'elle n'est pas en arriere-plan, enregistrez le PID du processus en avant-plan
            foregroundProcessCompleted = 0;
        }
        
        if (i != 0) {
            close(fd[i-1][1]);
        }
        
    }
    waitForForegroundProcess(l->background);

    freeDescripteurs(fd , nbPipes);
}

void closeAllPipe(int **fd , int nbPipe) {
    for (int j = 0; j < nbPipe; j++) {
        close(fd[j][0]);
        close(fd[j][1]);
    }
}