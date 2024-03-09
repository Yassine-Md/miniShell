#include "handler.h"
#include "process.h"

extern pid_t childpid;
extern int numJobs;
extern Job jobs[MAX_JOBS];
extern int numProcess ;
extern Process process[MAX_PROCESS];

void handlerSigInt(int sig) {
    fprintf(stderr, "Processus fils %d : Signal SIGINT reçu. Terminaison en cours...\n", getpid());
    kill(getpid(), SIGINT);
}


void processHandler(Process *process , int numProcess , pid_t pid , int status){

        // Recherche du job correspondant au PID dans le tableau des jobs
        for (int i = 0; i < numProcess; i++) {
            if (process[i].pid == pid) {
                // Mettre à jour le statut du processus dans le tableau des processus
                process[i].status = status;

                // Supprimer le processus du tableau des processus si terminé ou arrêté
                if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    // Afficher le message indiquant que le processus a été terminé
                    //printf("Processus [%d] Terminé\n", process[i].pid);

                    // Supprimer le processus du tableau des processus
                    removeProcess(childpid);
                } else if (WIFSTOPPED(status)) {
                    // Afficher le message indiquant que le processus a été arrêté
                    //printf("Processus [%d] Arrêté\n", process[i].pid);
                }

                // On a trouvé le processus correspondant au PID, on peut sortir de la boucle
                break;
            }
        }
}

void jobHandler(Job *jobs , int numJobs , pid_t pid , int status){
    // Recherche du job correspondant au PID dans le tableau des jobs
        for (int i = 0; i < numJobs; i++) {
            if (jobs[i].pid == pid) {
                jobs[i].status = status;

                // Si le processus fils a ete terminer normalement ou a cause d'un signal
                if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    // Afficher le message indiquant que le job a  ete terminer
                    //printf("[%d] Terminé %s\n", i + 1, jobs[i].command);

                    // Supprimer le job du tableau des jobs
                    removeJob(i,jobs);
                }
                // Si le processus fils a  ete arreter
                else if (WIFSTOPPED(status)) {
                    // Afficher le message indiquant que le job a ete arreter
                    //printf("[%d] Arrêté %s\n", i + 1, jobs[i].command);
                }

                // On a trouve le job correspondant au PID, on peut sortir de la boucle
                break;
            }
        }
}

void SigChildHandler(int sig) {
    pid_t pid;
    int status;

    // La boucle while sert a traiter tous les processus fils qui ont change d'etat
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        if (pid == -1) {
            perror("Error in waitpid");
            break;
        }
        processHandler(process, numProcess, pid, status);
        jobHandler(jobs, numJobs, pid, status);
    }
}



void handlerCtrlZ(int sig) {
    // Handle Ctrl+Z in the child processes
    //printf("Ctrl+Z received in the parent process. Adding child process %d to jobs.\n" , childpid);    
    // Ajouter le processus fils aux jobs
    addJob(childpid, "\0" , jobs); // il faut verifier que le ce processus n'existe pas dans jobs 
    // Envoyer le signal SIGSTOP au processus fils apres l'avoir ajoute aux jobs
    kill(childpid, SIGSTOP);
}




