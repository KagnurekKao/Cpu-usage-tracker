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


//semaphores
sem_t semEmpty;
sem_t semFull;

//mutexes
pthread_mutex_t mutexBuffer;


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
    while(1){
        //printf("Code for Reading Thread:\n");
        int r1 = rand()%10;
        int r2 = rand()%10;
        int r3 = rand()%10;
        
        sem_wait(&semFull);
        pthread_mutex_lock(&mutexBuffer);
        TemporaryFile[1] = r1;
        TemporaryFile[2] = r2;
        TemporaryFile[3] = r3;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmpty);

        printf("Reader: %d %d %d  | %d \n", r1, r2, r3, (r1+r2+r3)/3);
        usleep(100000);
    }
}

//Analising Thread
void* analising(void*arg){
    while(1){
        //printf("Code for Analising Thread:\n");
        sem_wait(&semFull);
        pthread_mutex_lock(&mutexBuffer);
        int a = (TemporaryFile[1]+TemporaryFile[2]+TemporaryFile[3])/3;
        TemporaryData = a;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmpty);

        printf("Analyzer: %d\n",a);
        usleep(100000);
        
       /* sem_wait(&semFull);
        pthread_mutex_lock(&mutexBuffer);
        
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmpty); */
    }
}

//Printing Thread
void* printing(void*arg){
    while(1){
        //printf("Code for Printing Thread:\n");
        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);
        int p = TemporaryData;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull);

        printf("Print: %d\n",p);  
        usleep(100000);
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