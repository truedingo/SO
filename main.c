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
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>

stats *stats_ptr;
config *config_ptr;
int shmid;
sem_t *sem_triage;
sem_t *sem_service;
sem_t *sem_waitb;
sem_t *sem_waite;
sem_t *sem_waiting;
sem_t *sem_control;
pid_t parent_pid;

void initialize_semaphores(){
    /*Dar unlink dos semaforos por questãos de segurança*/
    sem_unlink("sem_triage");
    sem_unlink("sem_service");
    sem_unlink("sem_waitb");
    sem_unlink("sem_waite");
    sem_unlink("sem_waiting");
    /*Dar open dos semaforos*/
    sem_triage = sem_open("sem_triage", O_CREAT | O_EXCL, 0700, 1);
    sem_service = sem_open("sem_service", O_CREAT | O_EXCL, 0700, 1);
    sem_waitb = sem_open("sem_waitb", O_CREAT | O_EXCL, 0700, 1);
    sem_waite = sem_open("sem_waite", O_CREAT | O_EXCL, 0700, 1);
    sem_waiting = sem_open("sem_waiting", O_CREAT | O_EXCL, 0700, 1);
}

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
    printf("Duracao do shift (Shift Length): %.1f\n", config_ptr->shift_length);
    printf("Message queue (MQ): %d\n", config_ptr->mq_max);
}
void *worker(){
    printf("Hello! I'm Triage thread!\n");
    sleep(2);
    printf("Triage thread is leaving... :(\n");
    pthread_exit(NULL);
}

void service_stats(){
    sem_wait(sem_service);
    (stats_ptr->num_service)++;
    sem_post(sem_service);
}

void triage_stats(){
    sem_wait(sem_triage);
    (stats_ptr->num_triage)++;
    sem_post(sem_triage);    
}

void thread_pool(){
    int i=0;
    pthread_t triage_thread[config_ptr->triage];
    int triage_id[config_ptr->triage];
    for(i=0; i<config_ptr->triage; i++){
        triage_id[i] = i;
        if(pthread_create(&triage_thread[i], NULL, worker, &triage_id[i])==0){
            printf("Triage thread %d was created\n", triage_id[i]);
            triage_stats();
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

void fork_call(){
    printf("Hello! I'm a doctor process, ready to help you!\n");
    service_stats();
    sleep(config_ptr->shift_length);
    printf("Well, my shift is over. Goodbye!\n");
}

void process_creator(){
	int i;
    pid_t pid;
    int forkValue;
	for(i=0; i<config_ptr->doctors; i++){
	    forkValue = fork();
        pid = getpid();
        parent_pid = getppid();
        if(forkValue == 0){
            printf("Doctor on service! My ID is %d\n", pid);
            fork_call();
            //LIBERTAR SEMAFORO -1
            exit(0);
        }
        else if (forkValue < 0){
            perror("Error creating process\n");
            exit(1);
        }
	}
    while(wait(NULL)>0);
}

void dynamic_processes(){
    //CRIAR SEMAFORO ESTE SEMAFOR PODE SER POSSUIDO 10 VEZES
    printf("Dynamically creating doctor processes\n");
    while(1){
        //POSSUIR SEMAFORO +1
        process_creator();
    }
}

void create_shared_memory(){
    if((shmid = shmget(IPC_PRIVATE, sizeof(stats), IPC_CREAT |0766)) == -1){
        perror("Error creating shared memory\n");   
        exit(1);
    }
    stats_ptr = (stats*) shmat(shmid, NULL, 0);
    printf("Shared memory sucessfully at address %p\n", stats_ptr);
    printf("Shared memory at %d\n", shmid);
    stats_ptr->num_triage=0;
    stats_ptr->num_service=0;
    stats_ptr->wait_btime=0;
    stats_ptr->wait_etime=0;
    stats_ptr->wait_time=0;
    /*Dados teste*/
    printf("Número de pacientes triados: %d\n", stats_ptr->num_triage);
    printf("Número de pacientes atendidos: %d\n", stats_ptr->num_service);
    printf("Tempo média de espera antes do inicio da triagem: %.1f\n", stats_ptr->wait_btime);
    printf("Tempo média de espera entre o fim da triagem e o início do atendimento: %.1f\n", stats_ptr->wait_etime);
    printf("Média do tempo total que cada paciente gastou desde que chegou ao sistema até sair: %.1f\n", stats_ptr->wait_time);

}

void stats_results(){
    printf("Final results:\n");
    printf("Número de pacientes triados: %d\n", stats_ptr->num_triage);
    printf("Número de pacientes atendidos: %d\n", stats_ptr->num_service);
    printf("Tempo média de espera antes do inicio da triagem: %.1f\n", stats_ptr->wait_btime);
    printf("Tempo média de espera entre o fim da triagem e o início do atendimento: %.1f\n", stats_ptr->wait_etime);
    printf("Média do tempo total que cada paciente gastou desde que chegou ao sistema até sair: %.1f\n", stats_ptr->wait_time);
}

void cleanup_sm(){
    if(shmdt(stats_ptr) == -1){
        perror("Error using shmdt\n");
    }
    printf("Sucessfully shmdt'd\n");
    if(shmctl(shmid, IPC_RMID, 0) == -1){
        perror("Error unmapping shared memory\n");
    }
    printf("Sucessfully shmctl'd\n");
}


void shutdown_semaphores(){
    sem_unlink("sem_triage");
    sem_unlink("sem_service");
    sem_unlink("sem_waitb");
    sem_unlink("sem_waite");
    sem_unlink("sem_waiting");

    sem_close(sem_triage);
    sem_close(sem_service);
    sem_close(sem_waitb);
    sem_close(sem_waite);
    sem_close(sem_waiting);
}


void signal_handler(int signum){
    /*while(wait(NULL)>0);*/
    stats_results();
    shutdown_semaphores();
    free(config_ptr);
    cleanup_sm();
    kill(0, SIGKILL);
}

void startup(){
    read_from_file();
    initialize_semaphores();
    create_shared_memory();
    thread_pool();
    dynamic_processes();
}

int main(){
    signal(SIGINT, signal_handler);
    config_ptr = malloc(sizeof(config));
    if(config_ptr == NULL){
        printf("Memory allocation error\n");
        return(1);
    }else{
        printf("Memory successfully allocated\n");
        }
    startup();
    printf("Exiting...\n");
    exit(0);
}
