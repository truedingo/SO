#include <stdlib.h>
#include <stdio.h>

/*Server.c*/
void read_from_file();
void thread_pool();
void *worker();
void kill_process();
void process_creator();
void initialize_semaphores();
void service_stats();
void triage_stats();
void fork_call();
void dynamic_processes();
void create_shared_memory();
void stats_results();
void cleanup_sm();
void shutdown_semaphores();
void signal_handler();
void startup();


/*Estrutura para config.txt
//triage -> nthreads
//doctors -> nprocesses
//shift -> Processes run time
//mq_max -> Maximum queue size*/

typedef struct config{
    int triage;
    int doctors;
    float shift_length;
    int mq_max;
}config;

/*Cria pointer para estrutura config*/
config *config_ptr;

/*Estrutura paciente
//name -> nome do paciente
//triage_time -> tempo de triagem
//service_time -> tempo de atendimento*/

typedef struct patient{
    /*por agora vou por aqui um char estatico just cuz*/
    char name[50];
    float triage_time;
    float service_time;
}patient;

/*Cria pointer para estrutura patient*/
patient *patient_ptr;

/*Estrutura para as estatísticas
//num_triage -> Número de pacientes triados
//num_service -> Número de pacientes atendidos
//wait_btime -> Tempo médio de espera antes do início triagem
//wait_etime -> Tempo médio de espera entre o fim da triagem e
//o início do atendimento
//wait_time -> Média do tempo total que cada paciente gastou desde
//que chegou ao sistema até sair*/

typedef struct stats{
    int num_triage;
    int num_service;
    float wait_btime;
    float wait_etime;
    float wait_time;
}stats;

/*Criar pointer para statistics*/
stats *stats_ptr;
