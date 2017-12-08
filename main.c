
#include "header.h"


stats *stats_ptr;
config *config_ptr;
int shmid;
int fd;
sem_t *sem_triage;
sem_t *sem_service;
sem_t *sem_waitb;
sem_t *sem_waite;
sem_t *sem_waiting;
sem_t *sem_control;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
    printf("----------READ FROM FILE----------\n");
    printf("\nNumero de threads (Triage): %d\n", config_ptr->triage);
    printf("\nNumero de processos (Doctors): %d\n", config_ptr->doctors);
    printf("\nDuracao do shift (Shift Length): %.1f\n", config_ptr->shift_length);
    printf("\nMessage queue (MQ): %d\n", config_ptr->mq_max);
    printf("----------------------------------\n");
    printf("\n");
}
void *worker(){
    pthread_mutex_lock(&mutex);

    pthread_mutex_unlock(&mutex);
    exit(0);
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

}

/*void kill_threads(){
    for(i=0; i<config_ptr->triage; i++){
        if(pthread_join(triage_thread[i], NULL)==0){
            printf("Triage thread %d was killed\n", triage_id[i]);
        }
        else{
            perror("Error killing Triage thread\n");
        }
    }
}*/

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
        if(forkValue == 0){
            signal(SIGINT,SIG_IGN);
            pid = getpid();
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
    /*Para a criação dinâmica, verificar message queue e os
    seus contéudos (se o que está na queue é >= 0,8) criar
    o processo temporário, ir verificando se a message queue
    desce dos 80% de capacidade*/
    /*Utilizar um if e depois um while para assegurar que o
    processo só se encontra ativo enquanto a message queue estiver
    no estado para tal*/
}

void while_processes(){
    //CRIAR SEMAFORO ESTE SEMAFOR PODE SER POSSUIDO 10 VEZES
    printf("Creating doctor processes\n");
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
    printf("----------SHARED MEMORY----------\n");
    printf("\nShared memory sucessfully at address %p\n", stats_ptr);
    printf("\nShared memory at %d\n", shmid);
    printf("---------------------------------\n");
    stats_ptr->num_triage=0;
    stats_ptr->num_service=0;
    stats_ptr->wait_btime=0;
    stats_ptr->wait_etime=0;
    stats_ptr->wait_time=0;
    /*Dados teste*/
    printf("----------RESULTS----------\n");
    printf("Número de pacientes triados: %d\n", stats_ptr->num_triage);
    printf("Número de pacientes atendidos: %d\n", stats_ptr->num_service);
    printf("Tempo média de espera antes do inicio da triagem: %.1f\n", stats_ptr->wait_btime);
    printf("Tempo média de espera entre o fim da triagem e o início do atendimento: %.1f\n", stats_ptr->wait_etime);
    printf("Média do tempo total que cada paciente gastou desde que chegou ao sistema até sair: %.1f\n", stats_ptr->wait_time);
    printf("---------------------------\n");

}

void stats_results(){
    printf("----------FINAL RESULTS----------\n");
    printf("Final results:\n");
    printf("Número de pacientes triados: %d\n", stats_ptr->num_triage);
    printf("Número de pacientes atendidos: %d\n", stats_ptr->num_service);
    printf("Tempo média de espera antes do inicio da triagem: %.1f\n", stats_ptr->wait_btime);
    printf("Tempo média de espera entre o fim da triagem e o início do atendimento: %.1f\n", stats_ptr->wait_etime);
    printf("Média do tempo total que cada paciente gastou desde que chegou ao sistema até sair: %.1f\n", stats_ptr->wait_time);
    printf("---------------------------------\n");
}

void cleanup_sm(){
    if(shmdt(stats_ptr) == -1){
        perror("Error using shmdt\n");
    }
    printf("Sucessfully shmdt'd\n");
    if(shmctl(shmid, IPC_RMID, NULL) == -1){
        perror("Error unmapping shared memory\n");
    }
    printf("Sucessfully shmctl'd\n");
}

void create_named_pipe(){
    if((mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST)){
        perror("Error creating named pipe: ");
        exit(0);
    }

    printf("Named pipe successfully created\n");

    if((fd = open(PIPE_NAME, O_RDONLY))< 0){
        perror("Error opening pipe for reading: ");
        exit(0);
    }
    /*

    Named pipe recebe dados e manda para uma queue (lista ligada)
    ler sobre merda dos selects e sets e o crl*/
    

}

PatientList create_patient_list(void){

    PatientList patient_node;
    patient_node = (PatientList) malloc(sizeof(node_Patient));

    if(patient_node != NULL){
        patient_node->next = NULL;
    }
    printf("List created\n");
    return patient_node;
}

int empty_patient_list(PatientList list){

    return (list->next == NULL ? 1 : 0);
}

PatientList destroy_patient_list(PatientList list){

    PatientList ptr;
    while(empty_patient_list(list) == 0){
        ptr = list;
        list = list->next;
        free(ptr);
    }
    free(list);
    return NULL;
}

void insert_patient(char name[30], float triage, float service, int priority, PatientList patients){
    PatientList newPatient;
    PatientList atual = patients;
    newPatient = malloc(sizeof(node_Patient));
    while(atual->next!= NULL){
        atual = atual->next;
    }
    strcpy(newPatient->patient.name, name);
    newPatient->patient.triage_time = triage;
    newPatient->patient.service_time = service;
    newPatient->patient.priority = priority;
    atual->next = newPatient;
    newPatient->next = NULL;

}

void list_patient(PatientList listaPacientes){
    PatientList patients;
    patients = listaPacientes->next;
    while(patients != NULL){
        printf("Nome: %s\n", patients->patient.name);
        printf("Triagem: %.1f\n", patients->patient.triage_time);
        printf("Service: %.1f\n", patients->patient.service_time);
        printf("Priority: %d\n", patients->patient.priority);
        patients = patients->next;
    } 
}

PatientList delete_patient_node(PatientList listaPacientes){
    PatientList aux = listaPacientes->next;
    listaPacientes->next = aux->next;
    free(aux);
}

Patient get_patient(PatientList listaPacientes, Patient returnedPatient){
    PatientList atual;
    atual = listaPacientes->next;
    if(empty_patient_list(listaPacientes)==1){
        printf("Empty list\n");
    }
    else{
        returnedPatient = atual->patient;
        delete_patient_node(listaPacientes);
        printf("RETURNED PATIENT:\n");
        printf("%s\n",returnedPatient.name);
        printf("%1f\n",returnedPatient.triage_time);
        printf("%1f\n",returnedPatient.service_time);
        printf("%d\n",returnedPatient.priority);
    }
    return returnedPatient;
}

void read_pipe(PatientList patients){
    
    char buffer[MAX];
    char name[50];
    float service;
    float triage;
    int priority;
    char checker[20];
    char date[30];
    char appender[30];
    time_t time_format;
    struct tm *my_time;
    int counter=0;
    int i = 1;
    while(1){
        read(fd, buffer, MAX);
        printf("Received: %s\n", buffer);
        for(int i=0; i< strlen(buffer); i++){
            if(buffer[i] == ';'){
                counter+=1;
            }
        }
        if(counter == 3){
            strcpy(checker, strtok(buffer, ";"));
            if (!isdigit(checker[0])){
                strcpy(name, checker);
                triage = atoi(strtok(NULL, ";"));
                service = atoi(strtok(NULL, ";"));
                priority = atoi(strtok(NULL, ";"));
                insert_patient(name, triage, service, priority, patients);

                }
            else{
                time(&time_format);
                my_time = localtime(&time_format);
                if(strftime(date, 30, "%Y%m%d", my_time)==0){
                    perror("Couldn't format string");
                }
                triage= atoi(strtok(NULL, ";"));
                service= atoi(strtok(NULL, ";"));
                priority= atoi(strtok(NULL, ";"));
                for(int k=1; k<=atoi(checker); k++){
                    strcpy(name, date);
                    sprintf(appender, "%d", i);
                    strcat(name, "-");
                    strcat(name, appender);
                    insert_patient(name, triage, service, priority, patients);
                    i+=1;
                }
                                
                    }
        }
        else{
            printf("Invalid input\n");
        }
        counter=0;
        list_patient(patients);
    }
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
    while(wait(NULL)>0);
    shutdown_semaphores();
    stats_results();
    cleanup_sm();
    /*kill_threads();*/
    free(config_ptr);
    kill(0, SIGKILL);
    exit(0);
}

void startup(){
    read_from_file();
    initialize_semaphores();
    create_shared_memory();
    thread_pool();
    while_processes();
}

int main(){
    /*signal(SIGINT, signal_handler);
    config_ptr = malloc(sizeof(config));
    if(config_ptr == NULL){
        printf("Memory allocation error\n");
        return(1);
    }else{
        printf("Memory successfully allocated\n");
        }
    startup();
    printf("Exiting...\n");
    exit(0);*/
    PatientList patients;
    Patient returnedPatient;
    patients = create_patient_list();
    create_named_pipe();
    read_pipe(patients);
    /*insert_patient("joao", 50, 10, 3, patients);
    insert_patient("artur", 50, 10, 3, patients);
    list_patient(patients);
    get_patient(patients, returnedPatient);
    list_patient(patients);*/
}
