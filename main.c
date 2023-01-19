/*******************************
 * CUT - CPU USAGE TRACKER
 * CREATED BY WIKTOR LIS
 * 
 * FOR: TIETOEVRY
 * LAST EDITED: 19.01.2023
********************************/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

sem_t semEmpty;
sem_t semFull;

pthread_mutex_t mutexBuffer;

int TemporaryFile[3];
int TemporaryData;
int count = 0;
int sum = 0;


void* reading();
void* analising();
void* printing();
 
//Main function
int main(int argc, char* argv[]){
    srand(time(NULL));
    pthread_t Reader, Analyzer, Printer;
    pthread_mutex_init(&mutexBuffer, NULL);
    sem_init(&semEmpty, 0, 1);
    sem_init(&semFull, 0 ,0);
    for(int i = 0; i < 3; i++){
        if (i==0){
            if (pthread_create(&Reader, NULL, &reading, NULL) !=0) {
                return printf("Failed to create a Reader");
            }
        } else if (i==1) {
            if (pthread_create(&Analyzer, NULL, &analising, NULL) !=0){
                return printf("Failed to create an Analyzer");
            }
        } else {    
            if (pthread_create(&Printer, NULL, &printing, NULL) !=0){
                return printf("Failed to create a Printer");
            }
        }    
    }
    for(int j = 0; j < 3; j++){
        if (j==0){
            if (pthread_join(Analyzer, NULL) !=0){
                return printf("Failed to join (Analyzer)");
            }    
        } else if (j==1) {
            if (pthread_join(Reader, NULL) !=0){
                return printf("Failed to join  (Reader)");
            }
        } else {    
            if (pthread_join(Printer, NULL) !=0){
                return printf("Failed to join (Printer)");
            }
        }
    }
    sem_destroy(&semEmpty);
    sem_destroy(&semFull);
    pthread_mutex_destroy(&mutexBuffer);
    printf("Main: program completed. Exiting.\n");
    pthread_exit(NULL); 
}

//Reading Thread
void* reading(){
    while(1){
        //printf("Code for Reading Thread:\n");
        int r1 = rand()%10;
        int r2 = rand()%10;
        int r3 = rand()%10;
        
        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);
        TemporaryFile[1] = r1;
        TemporaryFile[2] = r2;
        TemporaryFile[3] = r3;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull);

        printf("Reader: %d %d %d \n", r1, r2, r3);
        sleep(1);
    }
}

//Analising Thread
void* analising(){
    while(1){
        //printf("Code for Analising Thread:\n");
        sem_wait(&semFull);
        pthread_mutex_lock(&mutexBuffer);
        int a = (TemporaryFile[1]+TemporaryFile[2]+TemporaryFile[3])/3;
        
        printf("Analyzer %d\n",a);
        sleep(1);

        TemporaryData = a;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull);
    }
}

//Printing Thread
void* printing(){
    while(1){
        //printf("Code for Printing Thread:\n");
        sem_wait(&semFull);
        pthread_mutex_lock(&mutexBuffer);
        int p = TemporaryData;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmpty);

        printf("Print: %d\n",p);  
        usleep(300000);
    }
}