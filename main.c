/*******************************
 * CUT - CPU USAGE TRACKER
 * CREATED BY WIKTOR LIS
 * 
 * FOR: TIETOEVRY
 * LAST EDITED: 23.01.2023
********************************/
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include "sts_queue/sts_queue.h"

//sigterm
volatile sig_atomic_t done=0;

//semaphores
typedef struct
{
    char buf[3];
    sem_t occupied;
    sem_t empty;
    int nextin;
    int nextout;
    sem_t pmut;
    sem_t cmut;
} buffer_t;

buffer_t buffer;

//mutexes
pthread_mutex_t mutexBuffer;

//CPU usage math
char cpu[10];
long double p[5][10];
long double n[5][10];
long double cpuUsage[5];
int j,i;
long double PrevIdle, Idle, PrevNonIdle, NonIdle, PrevTotal, Total, totald, idled, CPU_Percentage;


//Functions
void* reading();
void* analising();
void* printing();
void term(int signum);

//Main function
int main(int argc, char* argv[]){

    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler=term;
    sigaction(SIGTERM,&action, NULL);

    pthread_t Reader, Analyzer, Printer;
    pthread_mutex_init(&mutexBuffer, NULL);

    sem_init(&buffer.occupied,0,0);
    sem_init(&buffer.empty,0,3);
    sem_init(&buffer.pmut,0,1);
    sem_init(&buffer.cmut,0,1);
    buffer.nextin = buffer.nextout = 0;
        
    while(!done){
        int t = sleep(1);
        while(t>0){
            printf("Interupted with %d sec to go, finishing...\n",t);
            t=sleep(t);
        }

        for(int i = 0; i < 3; i++){
            switch(i){
            case 0:
                if (pthread_create(&Reader, NULL, reading, NULL) !=0) {
                   return printf("Failed to create a Reader");
                }
                break;
            case 1:
                if (pthread_create(&Analyzer, NULL, analising, NULL) !=0){
                    return printf("Failed to create an Analyzer");
                }   
                break;    
            case 2: 
                if (pthread_create(&Printer, NULL, printing, NULL) !=0){
                    return printf("Failed to create a Printer");
                }         
                break;     
            }
        }
    }
}
    



//Reading Thread
void* reading(){
        FILE *f = fopen("/proc/stat","r");
        sem_wait(&buffer.empty);
        sem_wait(&buffer.pmut);
        pthread_mutex_lock(&mutexBuffer);
        for(j=0; j<5; j++)
        {
            fscanf(f, "%s", cpu);
            //printf("%s ", cpu);  
            for(i=0;i<10; i++)
            {     
                fscanf(f, "%Lf", &p[j][i]);
                //printf("%Lf ", p[j][i]);       
            } 
            //printf("\n");
        }
        sleep(1);
        rewind(f);
        for(j=0; j<5; j++)
        {
            fscanf(f, "%s", cpu);
            //printf("%s ", cpu);  
            for(i=0;i<10; i++)
            {     
                fscanf(f, "%Lf", &n[j][i]);
                //printf("%Lf ", n[j][i]);       
            } 
            //printf("\n");
        }
        fclose(f);
        //usleep(10000);
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&buffer.pmut);
        sem_post(&buffer.occupied);

}

//Analising Thread
void* analising(){
        sem_wait(&buffer.occupied);
        sem_wait(&buffer.cmut);
        pthread_mutex_lock(&mutexBuffer);
        for(j=0;j<5;j++)
        {    
            PrevIdle = p[j][3]+p[j][4];
            Idle = n[j][3]+n[j][4];
            PrevNonIdle = n[j][0]+n[j][1]+n[j][2]+n[j][5]+n[j][6]+n[j][7];
            NonIdle = p[j][0]+p[j][1]+p[j][2]+p[j][5]+p[j][6]+p[j][7];
            PrevTotal = PrevIdle + PrevNonIdle;
            Total = Idle + NonIdle;
            totald = Total - PrevTotal;
            idled = Idle - PrevIdle;
            CPU_Percentage = ((-1)*(totald-idled)/totald)*100;
            cpuUsage[j]=CPU_Percentage;            
        }
        //usleep(10000);
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&buffer.cmut);
        sem_post(&buffer.empty);
        /*
        sem_wait(&buffer.empty);
        sem_wait(&buffer.pmut);
        pthread_mutex_lock(&mutexBuffer);

        usleep(10000);
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&buffer.pmut);
        sem_post(&buffer.occupied);
       
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
        */
}

//Printing Thread
void* printing(){
        sem_wait(&buffer.occupied);
        sem_wait(&buffer.cmut);
        pthread_mutex_lock(&mutexBuffer);
        for(i=0; i<5;i++)
        {
        if(i==0){
        //printf("|CPU usage: %Lf%c |", cpuUsage[i],'%');  
        }else
        {
        printf("|CPU %d usage: %Lf%c |",i-1, cpuUsage[i],'%'); 
        }
        }
        printf("\n");   
        //usleep(10000);
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&buffer.cmut);
        sem_post(&buffer.empty);
        //sem_post(&semEmpty);
        /*
        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);
        printf("CPU_percentage: %.2Lf\n",CPU_Percentage);
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull); 
        */
}

void term(int signum){
    done =1;
    printf("Terminating Cpu usage tracker\n");
    sem_destroy(&buffer.occupied);
    sem_destroy(&buffer.empty);
    sem_destroy(&buffer.cmut);
    sem_destroy(&buffer.pmut);
    pthread_mutex_destroy(&mutexBuffer);
    pthread_exit(NULL);
    exit(signum);
}