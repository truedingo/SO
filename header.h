#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <semaphore.h>
#include <ctype.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/timeb.h>
#define MAX 50
#define PIPE_NAME "input_pipe"


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
//service_time -> tempo de atendimento
//priority -> prioridade de atendimento*/

typedef struct patient{
    /*por agora vou por aqui um char estatico just cuz*/
    char name[50];
    float triage_time;
    float service_time;
    int priority;
}Patient;

/*Cria pointer para estrutura patient*/
Patient *patient_ptr;

/*Struct para lista ligada de Pacientes*/
typedef struct nodePatient *PatientList;
typedef struct nodePatient{
    Patient patient;
    PatientList next;
}node_Patient;

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
    int mq_size;
}stats;

/*Criar pointer para statistics*/
stats *stats_ptr;

/*Estrutura para a message queue*/
typedef struct mensagem{
    long mtype;
    Patient pat;
}msg;

/*Server.c*/
void initialize_semaphores();
void read_from_file();
void create_message_queue();
void cleanup_mq();
void *worker(void* pVoid);
void service_stats();
void triage_stats();
void decrease_mq();
void increase_mq();
void thread_pool();
void fork_call();
void process_creator();
void create_shared_memory();
void stats_results();
void cleanup_sm();
void create_named_pipe();
PatientList create_patient_list(void);
int empty_patient_list(PatientList list);
PatientList destroy_patient_list(PatientList list);
void insert_patient(char name[30], float triage, float service, int priority, PatientList patients);
void list_patient(PatientList listaPacientes);
void delete_patient_node(PatientList listaPacientes);
Patient get_patient(PatientList listaPacientes, Patient returnedPatient);
void *read_pipe();
void shutdown_semaphores();
void signal_handler(int signum);






