/*******************************
 * CUT - CPU USAGE TRACKER
 * CREATED BY WIKTOR LIS
 * 
 * FOR: TIETOEVRY
 * LAST EDITED: 20.01.2023
********************************/
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include "sts_queue/sts_queue.h"
#include "sts_queue/sts_queue.c"


//semaphores
sem_t semEmpty;
sem_t semFull;

//mutexes
pthread_mutex_t mutexBuffer;

//CPU usage math
long double a[8],p[8];
long double PrevIdle, Idle, PrevNonIdle, NonIdle, PrevTotal, Total, totald, idled, CPU_Percentage;

//threads
int TemporaryFile[3]; //removce later
int *TemporaryData; // remove later


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
    
    /*for(int j = 0; j < 3; j++){
        if (j==0){
            elems[j]=pthread_join(Reader, NULL);
            StsQueue.push(handle, &elems[j]);
            if (pthread_join(Reader, NULL) !=0){
                return printf("Failed to join (Analyzer)");
            }    
            int *mem;
            while((mem = StsQueue.pop(handle)) !=NULL){
                printf("%i\n", *mem);
            }    
    
        } else if (j==1) {
            elems[j]=pthread_join(Analyzer, NULL);
            StsQueue.push(handle, &elems[j]);
            if (pthread_join(Analyzer, NULL) !=0){
                return printf("Failed to join  (Reader)");
            }
            int *mem;
            while((mem = StsQueue.pop(handle)) !=NULL){
                printf("%i\n", *mem);
            }  
        } else {   
            elems[j]=pthread_join(Printer, NULL);
            StsQueue.push(handle, &elems[j]); 
            if (pthread_join(Printer, NULL) !=0){
                return printf("Failed to join (Printer)");
            }
            int *mem;
            while((mem = StsQueue.pop(handle)) !=NULL){
                printf("%i\n", *mem);
            }  
        }
    } */

    StsQueue.destroy(handle);
    sem_destroy(&semEmpty);
    sem_destroy(&semFull);
    pthread_mutex_destroy(&mutexBuffer);
    pthread_exit(NULL); 
    
    
}
    

//Reading Thread
void* reading(void*arg){
    
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
void* analising(void*arg){
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
        CPU_Percentage = ((totald-idled)/totald)*100;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmpty);
    }
}

//Printing Thread
void* printing(void*arg){
    while(1){
        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);
        printf("CPU_percentage: %Lf\n",CPU_Percentage);
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull); 
    }
}

/*
int count = 0;
int queue[3];

//Add a thread into queue
void instertIntoQueue(int x) {
    if (count == 256){
        fprintf(stderr, "No more space in the queue\n");
        return;
    }
    queue[count] = x;
    count++;
}

//Remove a thread from queue
int removeFromQueue(){
    if (count == 0){
        fprintf(stderr, "No threads to extract from queue");
        return -1;
    }
    int result = queue[0];
    for(int i = 0; i < count - 1; i++){
        queue[i] = queue[i+1];     
    }
    count--;
    return result;
}
*/
/*ring buffer
int bufferLength = 0;
int readIndex = 0;
int writeIndex = 0;


void RingBuffer(){
    if (bufferLength == SIZE_OF_BUFFER){
        printf("Buffer is full");
    }   
        //getThread(circularbuffer[write]index]);
        switch (writeIndex){
            case 0:
                ;
            case 1:;
            case 2:;
        }
        //getThread(circularbuffer[write]index]);
        bufferLength++;
        writeIndex++;
    if (writeIndex == SIZE_OF_BUFFER){
        writeIndex = 0;
    }
    if (bufferLength == 0){
        printf("Buffer is empty!");
    }
}
*/