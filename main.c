/*******************************
 * CUT - CPU USAGE TRACKER
 * CREATED BY WIKTOR LIS
 * 
 * FOR: TIETOEVRY
 * LAST EDITED: 22.01.2023
********************************/
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include "sts_queue/sts_queue.h"
//#include "sts_queue/sts_queue.c"

//semaphores
sem_t semEmpty;
sem_t semFull;

//mutexes
pthread_mutex_t mutexBuffer;

//CPU usage math
long double a[8],p[8];
long double PrevIdle, Idle, PrevNonIdle, NonIdle, PrevTotal, Total, totald, idled, CPU_Percentage;


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
    
    StsHeader *handle = StsQueue.create();
    int elems[6];
    for(int i = 0; i < 3; i++){
        if (i==0){
            if (pthread_create(&Reader, NULL, &reading, NULL) !=0) {
                return printf("Failed to create a Reader");
            }  
            elems[i]=pthread_create(&Reader, NULL, &reading, NULL);
            StsQueue.push(handle, &elems[i]);
        } else if (i==1) {
            if (pthread_create(&Analyzer, NULL, &analising, NULL) !=0){
                return printf("Failed to create an Analyzer");
            }     
            elems[i]=pthread_create(&Reader, NULL, &reading, NULL);
            StsQueue.push(handle, &elems[i]);         
        } else {   
            if (pthread_create(&Printer, NULL, &printing, NULL) !=0){
                return printf("Failed to create a Printer");
            }           
            elems[i]=pthread_create(&Reader, NULL, &reading, NULL);
            StsQueue.push(handle, &elems[i]);  
        }    
        int *mem;
        while((mem = StsQueue.pop(handle)) !=NULL){
            if (*mem==0){
               
            }
            else{
                printf("%i\n", *mem);
            }
        }
    }

    StsQueue.destroy(handle);
    sem_destroy(&semEmpty);
    sem_destroy(&semFull);
    pthread_mutex_destroy(&mutexBuffer);
    pthread_exit(NULL); 
   
}
    
//Reading Thread
void* reading(){
    
    FILE *fp;

    while(1)
    {
        fp = fopen("/proc/stat", "r");
        sem_wait(&semFull);
        pthread_mutex_lock(&mutexBuffer);
        fscanf(fp,"%*s %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf ", &p[0], &p[1], &p[2], &p[3], &p[4], &p[5], &p[6], &p[7]);
        fclose(fp);
        //sleep(1);
        
        sleep(1);;
        fp = fopen("/proc/stat", "r");
        fscanf(fp,"%*s %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf ", &a[0], &a[1], &a[2], &a[3], &a[4], &a[5], &a[6], &a[7]);
        fclose(fp);
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmpty);   
    }
}

//Analising Thread
void* analising(){
    while(1){
        sem_wait(&semFull);
        pthread_mutex_lock(&mutexBuffer);
        PrevIdle = p[3]+p[4];
        Idle = a[3]+a[4];
        PrevNonIdle = a[0]+a[1]+a[2]+a[5]+a[6]+a[7];
        NonIdle = p[0]+p[1]+p[2]+p[5]+p[6]+p[7];
        PrevTotal = PrevIdle + PrevNonIdle;
        Total = Idle + NonIdle;
        totald = Total-PrevTotal;
        idled = Idle - PrevIdle;
        CPU_Percentage = ((-1)*(totald-idled)/totald)*100;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmpty);
    }
}

//Printing Thread
void* printing(){
    while(1){
        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);
        printf("CPU_percentage: %.2Lf\n",CPU_Percentage);
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull); 
    }
}
