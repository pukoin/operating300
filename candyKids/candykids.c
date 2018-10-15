#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdint.h>
#include "bbuff.h"
#include "stats.h"

#define NUMARGS 3

typedef struct  {
    int factory_number; //tracks which factory thread produced the candy item.
    double time_stamp_in_ms; //tracks when the item was created. You can get the current number of milliseconds using the following function. 
} candy_t;

_Bool stop_thread_factories = false;
//_Bool stop_thread_kids = false;

void* factory_thread(void* arg);
void* kid_thread(void* arg);

int main(int argc, char* argv[]) {
    // 1.  Extract arguments
    // 2.  Initialize modules
    // 3.  Launch candy-factory threads
    // 4.  Launch kid threads
    // 5.  Wait for requested time
    // 6.  Stop candy-factory threads
    // 7.  Wait until no more candy
    // 8.  Stop kid threads
    // 9.  Print statistics
    // 10. Cleanup any allocated memory





    // 1.  Extract arguments

    //4 because first argument is a pointer
    if(argc != 4) {
		printf("Invalid number of arguments, exiting\n");
        exit(1);
	}

    //checking for args less than 1
	for(int i=1; i<=NUMARGS; i++) {
		if(atoi(argv[i])<1) {
			printf("Invalid , exiting\n");
            exit(1);
		}
	}

    int inputArgs[3];
    for(int i=1; i<=NUMARGS; i++){
		sscanf(argv[i], "%d", &inputArgs[i-1]);
    }
    int factories = inputArgs[0];
    int kids = inputArgs[1];
    int seconds = inputArgs[2];

 /*   printf("%d\n",factories);
    printf("%d\n",kids);
    printf("%d\n",seconds);*/


    
    pthread_t* factoryThreads = malloc(factories *(sizeof(pthread_t)));
    pthread_t* kidThreads = malloc(kids *(sizeof(pthread_t)));
    //pthread_t* factoryThreads[factories];
    //pthread_t* kidThreads[kids];


    // 2.  Initialize modules
    
    //Passing in the number of factories
    stats_init(factories);
    bbuff_init();

     // 3.  Launch candy-factory threads
     //Spawn the requested number of candy-factory threads. To each thread, pass it its factory number: 0 to (number of factories - 1).

    //int error = 0;
    pthread_t tid;
    //pthread_attr_t attr;

    //stop_thread = false;
        
    int error = 0;

    for(int i = 0; i<factories; i++){

        //pthread_attr_init(&attr);  // default attributes 
	    error = pthread_create(&tid,NULL,factory_thread, (void*)(intptr_t)i);  

        if(error != 0){
			printf("Failed to launch thread, exiting\n");
			exit(-1);
        }

 	    //pthread_join(tid, NULL);

        //factoryThreads[i] = &tid; 
        factoryThreads[i] = tid;
    }
 
     // 4.  Launch kid threads

     error = 0;

    for(int i = 0; i<kids; i++){

        //pthread_attr_init(&attr);  // default attributes 
	    error = pthread_create(&tid,NULL,kid_thread, (void*)(intptr_t)i);  

        if(error != 0){
			printf("Failed to launch thread, exiting\n");
			exit(-1);
        }

 	    //pthread_join(tid, NULL);

        //kidThreads[i] = &tid; 
        kidThreads[i] = tid; 
    }

     //5. Wait for requested time
	for(int i=0; i<seconds; i++) {
		sleep(1);
		printf("Time: %ds\n", i);
    }

     //6. Stop candy-factory threads

     stop_thread_factories = true;
     printf("Stopping candy factories...\n");
     for(int i=0; i<factories; i++) {
        //printf("stopping Candy factory %d\n",i);
        //pthread_cancel(*factoryThreads[i]);  
		pthread_join(factoryThreads[i], NULL);
        //printf("Candy factory %d stopped!\n",i);
        //unction waits for the thread specified by thread to terminate. 
    }
    
    //printf("Finished stopping Candy factories\n");

    // 7.  Wait until no more candy

    //printf("dadsadsad\n");

	while(!bbuff_is_empty()) {
        //printf("not empty\n");
		sleep(1);
        printf("Waiting for all candy to be consumed.\n");
        //printf("312312312312\n");
        //printf("not empty\n");
    }

    // 8.  Stop kid threads

    printf("Stopping kids.\n");
    //stop_thread_kids = true;

     for(int i=0; i<kids; i++) {
        //printf("stopping kid %d\n",i);
        //printf("65656565\n");
        pthread_cancel(kidThreads[i]); 
		//pthread_join(kidThreads[i], NULL);
    }
    //printf("dasdasdasdasdasdasdasdsa\n");
    // 9.  Print statistics

    stats_display();

    // 10. Cleanup any allocated memory
    //printf("here.\n");
    stats_cleanup();
    //printf("there.\n");
	free(factoryThreads);
	free(kidThreads);
    //stats_cleanup();

    return 0;
}

void* factory_thread(void* arg){

    int i = (intptr_t)arg;

    int wait_time;
    while(stop_thread_factories == false){

        wait_time = rand()%4;   //wait times will be from 0 to 3
        printf("\tFactory %d ships candy & waits %ds\n", i, wait_time);

        //following steps in the outline in the assignment

        candy_t* newCandy = malloc(sizeof(candy_t));
        newCandy->time_stamp_in_ms = current_time_in_ms();
		newCandy->factory_number = i;
        //printf("about to insert\n");
        bbuff_blocking_insert(newCandy);
        //printf("done inserting\n");
		stats_record_produced(i);
        sleep(wait_time);
    }

    printf("Candy-factory %d done\n", i);
	
    pthread_exit(NULL);

    //printf("Just finished the exit call\n");

    //return NULL;
}


void* kid_thread(void* arg){


    //printf("entered child!\n");

    //int i = (intptr_t)arg;

    int wait_time;
   while(true){

        wait_time = rand()%2;   //can be either 0 or 1
        candy_t* newCandy = (candy_t*)bbuff_blocking_extract(); //cast to pointer to candy_t
        //printf("\tKid %d eats candy & waits %ds\n", i, wait_time);

        //newCandy->time_stamp_in_ms = current_time_in_ms();
		//newCandy->factory_number = i;

        if(newCandy!=NULL) {
            stats_record_consumed(newCandy->factory_number, (current_time_in_ms()-newCandy->time_stamp_in_ms));
        }

		free(newCandy);
        sleep(wait_time);

    }

    pthread_exit(NULL);

    //printf("child done!\n");

    //return NULL;
}