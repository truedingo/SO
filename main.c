#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "header.h"

/*Função que le do ficheiro e armazena na estrutura*/
void read_from_file(){
    FILE *fp = fopen("config.txt", "r");
    if (!fp){
        perror("Error reading the file\n");
        exit(0);
    }
    else
        printf("Sucessfully read the file\n");
    char triage[10], doctors[10], shift[10], mq[10];

    fscanf(fp, "TRIAGE=%s\n", triage);
    fscanf(fp, "DOCTORS=%s\n", doctors);
    fscanf(fp, "SHIFT_LENGTH=%s\n", shift);
    fscanf(fp, "MQ_MAX=%s\n", mq);

    (config_ptr->triage) = atoi(triage);
    (config_ptr->doctors) = atoi(doctors);
    (config_ptr->shift_length) = atoi(shift);
    (config_ptr->mq_max) = atoi(mq);
    fclose(fp);

    /*Teste*/
    printf("Numero de threads (Triage): %d\n", config_ptr->triage);
    printf("Numero de processos (Doctors): %d\n", config_ptr->doctors);
    printf("Duracao do shift (Shift Length): %f\n", config_ptr->shift_length);
    printf("Message queue (MQ): %d\n", config_ptr->mq_max);
}
void *worker(){
    printf("Hello! I'm Triage thread!\n");
    sleep(2);
    printf("Triage thread is leaving... :(\n");
    pthread_exit(NULL);
}

void thread_pool(){
    int i=0;
    pthread_t triage_thread[config_ptr->triage];
    int triage_id[config_ptr->triage];
    for(i=0; i<config_ptr->triage; i++){
        triage_id[i] = i;
        if(pthread_create(&triage_thread[i], NULL, worker, &triage_id[i])==0){
            printf("Triage thread %d was created\n", triage_id[i]);
        }
        else{
            perror("Error creating Triage thread\n");
        }
    }
    for(i=0; i<config_ptr->triage; i++){
        if(pthread_join(triage_thread[i], NULL)==0){
            printf("Triage thread %d was killed\n", triage_id[i]);
        }
        else{
            perror("Error killing Triage thread\n");
        }
    }

}

void doctor_pool(){
	int i;
    int forkValue;
	for(i=0; i<configptr->doctor: i++){
	    forkValue = fork();
        if(forkValue == 0){
            printf("Doctors on service.");
        }
				
	}
}

int main(){
    config_ptr = malloc(sizeof(config));
    if(config_ptr == NULL){
        printf("Memory allocation error\n");
        return(-1);
    }else{
        printf("Memory successfully allocated\n");
        }
    read_from_file();
    thread_pool();
    printf("Exiting...\n");
}
