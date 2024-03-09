#include "job.h"
#include <stdio.h>

extern int numJobs;

void removeJob(int index , Job *jobs) {
    if (index >= 0 && index < numJobs) {
        jobs[index].pid = 0;
        for (int i = index; i < numJobs - 1; i++) {
            jobs[i] = jobs[i + 1];
        }
        numJobs--;
    }
}

void addJob(pid_t pid, char* command , Job *jobs) {
    // verifeir qu'il y a pas des jobs plus que c'est permis
    if (numJobs < MAX_JOBS) {
        jobs[numJobs].pid = pid;

        // Utilisez WIFSTOPPED pour verifier si le processus est en pause
        if (WIFSTOPPED(jobs[numJobs].status)) {
            jobs[numJobs].status = JOB_STATUS_STOPPED; 
        }else{ // running
            jobs[numJobs].status = JOB_STATUS_RUNNING;
        }
        // pour des fin de securite on utilise strncpy
        strncpy(jobs[numJobs].command, command, MAX_COMMAND_LENGTH);
        numJobs++;
    } else {
        fprintf(stderr, "Nombre maximal de jobs atteint\n");
    }
}


void printJobs(Job *jobs) {
    for (int i = 0; i < numJobs; i++) {
            printf("[%d] %d %s\n", i + 1, jobs[i].pid, jobs[i].command);
    }
}