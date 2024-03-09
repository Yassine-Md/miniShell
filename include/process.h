#ifndef __PROCESS_H__
#define __PROCESS_H__

#define MAX_COMMAND_LENGTH 100
#define MAX_PROCESS 10
#define PROCESS_STATUS_STOPPED 0
#define PROCESS_STATUS_RUNNING 1
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

typedef struct {
    pid_t pid;        // PID du processus
    int status;       // Statut du processus (terminer, en cours, etc.)
    char command[MAX_COMMAND_LENGTH];  // Commande du processus
} Process;

int findProcessIndex(pid_t childpid);
int waitForForegroundProcess(Process *process);
void removeProcess(pid_t childpid) ;
void addProcess(pid_t pid, char* command);
void printProcess();

#endif