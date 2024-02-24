#ifndef __JOB_H__
#define __JOB_H__

#include <stdlib.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_JOBS 10
#define JOB_STATUS_STOPPED 0
#define JOB_STATUS_RUNNING 1

// Structure pour stocker les informations sur un job
typedef struct {
    pid_t pid;        // PID du processus
    int status;       // Statut du processus (terminer, en cours, etc.)
    char command[MAX_COMMAND_LENGTH];  // Commande du processus
} Job;


void removeJob(int index);
void addJob(pid_t pid, char* command);
void printJobs();



#endif