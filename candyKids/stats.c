#include "stats.h"
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>


typedef struct stats
{
	int factoryNum;
	int made;
	int eaten;
	double minDelay;
	double avgDelay;
	double maxDelay;
    double totalDelay;
} stats_t;

stats_t *factoryStats;
sem_t statsmutex;

int numFactories = 0;

//producers are the factories and the consumers are the kids
void stats_init(int num_producers){

    //int error = 0;
    factoryStats = malloc(num_producers*sizeof(stats_t));
    for(int i = 0; i<num_producers;i++){
        factoryStats[i].factoryNum = i;
		factoryStats[i].made = 0;
		factoryStats[i].eaten = 0;
		factoryStats[i].minDelay = -1;
		factoryStats[i].avgDelay = -1;
		factoryStats[i].maxDelay = -1;
        factoryStats[i].totalDelay = 0;
    }
    sem_init(&statsmutex, 0, 1);
    //set it to 1 without allowing other processes to share it


    numFactories = num_producers; 
}

void stats_cleanup(void){
free(factoryStats);
factoryStats = NULL;
}

void stats_record_produced(int factory_number){
    sem_wait(&statsmutex);
	factoryStats[factory_number].made++;
    sem_post(&statsmutex);
}

void stats_record_consumed(int factory_number, double delay_in_ms){

    sem_wait(&statsmutex);	//this mutex controls access to the stats

	factoryStats[factory_number].eaten++;		//increment the candy eaten, must match the candy produced
	if(factoryStats[factory_number].minDelay == -1) {
        factoryStats[factory_number].maxDelay = delay_in_ms;
		factoryStats[factory_number].totalDelay = delay_in_ms;
		factoryStats[factory_number].minDelay = delay_in_ms;
		factoryStats[factory_number].avgDelay = delay_in_ms;
	}
	else {
		if(factoryStats[factory_number].minDelay > delay_in_ms) {
			factoryStats[factory_number].minDelay = delay_in_ms;
		}
		if(factoryStats[factory_number].maxDelay < delay_in_ms) {
			factoryStats[factory_number].maxDelay = delay_in_ms;
		}
		factoryStats[factory_number].totalDelay += delay_in_ms;
		factoryStats[factory_number].avgDelay = factoryStats[factory_number].totalDelay/(double)factoryStats[factory_number].eaten;
	}

    sem_post(&statsmutex);

}

void stats_display(void){

    printf("Statistics: \n");

    //printf("%s%10s%11s%18s%18s\n", "Factory #", "# Made", "# Eaten", "Min Delay [ms]", "Max Delay [ms]");
	printf("%8s%10s%10s%20s%20s%20s\n", "Factory#", "#Made", "#Eaten", "Min Delay[ms]", "Avg Delay[ms]", "Max Delay[ms]");
	for(int i=0; i<numFactories; i++) {
		printf("%8d%10d%10d%20.5f%20.5f%20.5f\n",factoryStats[i].factoryNum,factoryStats[i].made,factoryStats[i].eaten,factoryStats[i].minDelay,factoryStats[i].avgDelay, factoryStats[i].maxDelay);
		if(factoryStats[i].made != factoryStats[i].eaten) {
			printf("\tERROR: Mismatch between number made and eaten.\n");
		}
	}
    printf("\n");
    

}