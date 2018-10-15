#include "bbuff.h"
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>


void* buff[BUFFER_SIZE];
int current_Location;   //current index of most recent element in the buffer

//Semaphores & Mutex
sem_t num_full_buffers;  //describes the full buffers, initalized to BUFFER_SIZE = 10
sem_t num_empty_buffers;    //describes the empty buffer, initialized to 0
sem_t mutex;    //controls access to checking the current location


double current_time_in_ms(void)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}

void bbuff_init(void){
    current_Location = 0; 
    sem_init(&num_full_buffers,0,0);
    sem_init(&num_empty_buffers,0,BUFFER_SIZE);
    sem_init(&mutex,0,1);   //binary semaphore cause one person can access at a time

}


void bbuff_blocking_insert(void* item){

    //int tempvalue = 0;

    //sem_getvalue((&num_empty_buffers), &tempvalue);
    //printf("num_empty_buffers value before sem_wait call is  %d\n",tempvalue);

    //does the order matter? not sure 
    sem_wait(&num_empty_buffers);

    //sem_getvalue((&num_empty_buffers), &tempvalue);
    //printf("num_empty_buffers value after sem_wait call is  %d\n",tempvalue);

    //sem_getvalue((&mutex), &tempvalue);
    //printf("mutex value before sem_wait call is  %d\n",tempvalue);

    sem_wait(&mutex);

    //sem_getvalue((&mutex), &tempvalue);
    //printf("mutex value after sem_wait call is  %d\n",tempvalue);

    //printf("Current Location being inserted in; %d\n",current_Location);

    buff[current_Location] = item;
    current_Location = (current_Location + 1);//%BUFFER_SIZE;//want it to loop

    sem_post(&mutex);
    //printf("Mutex releaseded by factory\n");
    sem_post(&num_full_buffers);

}


void* bbuff_blocking_extract(void){

    //int tempvalue = 0;

    //printf("Entered extraction\n");

    //sem_getvalue((&num_full_buffers), &tempvalue);
    //printf("num_full_buffers value before sem_wait call is  %d\n",tempvalue);

    sem_wait(&num_full_buffers);

    //sem_getvalue((&num_full_buffers), &tempvalue);
    //printf("num_full_buffers value before sem_wait call is  %d\n",tempvalue);


    //printf("Waiting for the mutex\n");

    sem_wait(&mutex);


    //printf("Current Location being extracted from; %d\n",current_Location - 1);

    current_Location = (current_Location - 1);//%BUFFER_SIZE;

    //CHANGED THIS TO current_Location - 1 PREVIOUSLY
    void* currentCandy = buff[current_Location];
    buff[current_Location] = NULL;
    //current_Location = (current_Location - 1)%BUFFER_SIZE;

    sem_post(&mutex);
    sem_post(&num_empty_buffers);

    return currentCandy;
}


_Bool bbuff_is_empty(void){
    return(current_Location == 0);
}