#include <stdlib.h>
#include <stdio.h>

/*Server.c*/
void read_from_file();
void thread_pool();

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

/*Estrutura para as estat�sticas
//num_triage -> N�mero de pacientes triados
//num_service -> N�mero de pacientes atendidos
//wait_btime -> Tempo m�dio de espera antes do in�cio triagem
//wait_etime -> Tempo m�dio de espera entre o fim da triagem e
//o in�cio do atendimento
//wait_time -> M�dia do tempo total que cada paciente gastou desde
//que chegou ao sistema at� sair*/

typedef struct stats{
    int num_triage;
    int num_service;
    float wait_btime;
    float wait_etime;
    float wait_time;
}stats;

/*Criar pointer para statistics*/
stats *stats_ptr;
