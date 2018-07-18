#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <pthread.h>
#include <assert.h>

/* NO NEED TO MODIFY */

#define SERVINGS 100000

typedef struct{
  int holds_left;
  int holds_right;
  int is_eating;
  int is_thinking;
} philosopher_t;


static philosopher_t philosopher[5] = {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
static int meals_eaten[5] = {0,0,0,0,0};


static double const thinking_to_eating = 0.02;
static double const eating_to_thinking = 0.05;

static unsigned int seed[5];


/*

chopstick helper functions

*/

pthread_mutex_t chopsticks[5];
/*
 * Performs necessary initialization of mutexes.
 */
void chopsticks_init(){
 	int i=0;
	for (i=0;i<5;i++){
		pthread_mutex_init(&chopsticks[i],NULL);
	}
}

/*
 * Cleans up mutex resources.
 */
void chopsticks_destroy(){
	int i=0;
	for (i=0;i<5;i++){
		pthread_mutex_destroy(&chopsticks[i]);
	}
}

/*
 * Uses pickup_left_chopstick and pickup_right_chopstick
 * to pick up the chopsticks
 */


void pickup_left_chopstick(int phil_id){
	assert(!philosopher[(phil_id+4) % 5].holds_right);
  philosopher[phil_id].holds_left = 1;
}

void putdown_left_chopstick(int phil_id){
	assert(philosopher[phil_id].holds_left);
  philosopher[phil_id].holds_left = 0;
}

void pickup_right_chopstick(int phil_id){
	assert(!philosopher[(phil_id+1) % 5].holds_left);
  philosopher[phil_id].holds_right = 1;
}

void putdown_right_chopstick(int phil_id){
	assert(philosopher[phil_id].holds_right);
  philosopher[phil_id].holds_right = 0;
}

void start_eating(int phil_id){
	assert(philosopher[phil_id].holds_right);
	assert(philosopher[phil_id].holds_left);
  philosopher[phil_id].is_eating = 1;
}

void stop_eating(int phil_id){
	assert(philosopher[phil_id].holds_right);
	assert(philosopher[phil_id].holds_left);
	meals_eaten[phil_id]++;
  philosopher[phil_id].is_eating = 0;
}

int count_meals_eaten(int phil_id){
	return meals_eaten[phil_id];
}


void pickup_chopsticks(int phil_id){
	if(phil_id == 4) {
		pthread_mutex_lock(&chopsticks[(phil_id + 1) % 5]);
		pickup_right_chopstick(phil_id);
		pthread_mutex_lock(&chopsticks[(phil_id) % 5]);
		pickup_left_chopstick(phil_id);
	}
	else {
		pthread_mutex_lock(&chopsticks[(phil_id) % 5]);
		pickup_left_chopstick(phil_id);
		pthread_mutex_lock(&chopsticks[(phil_id+1)%5]);
		pickup_right_chopstick(phil_id);
	}
}

/*
 * Uses putdown_left_chopstick and putdown_right_chopstick
 * to put down the chopsticks
 */   
void putdown_chopsticks(int phil_id){
	if(phil_id == 4){
		putdown_right_chopstick(phil_id);
		pthread_mutex_unlock(&chopsticks[(phil_id + 1) % 5]);
		putdown_left_chopstick(phil_id);
		pthread_mutex_unlock(&chopsticks[(phil_id) % 5]);
	}
	else {
		putdown_left_chopstick(phil_id);
		pthread_mutex_unlock(&chopsticks[(phil_id) % 5]);
		putdown_right_chopstick(phil_id);
		pthread_mutex_unlock(&chopsticks[(phil_id + 1) % 5]);
	}
}


/*

    philosopher helper functions

*/








static void eat(int phil_id){
  start_eating(phil_id);
  while ( (rand_r(&seed[phil_id]) / (RAND_MAX + 1.0)) >= eating_to_thinking);
  stop_eating(phil_id);
}

static void think(int phil_id){
  while ((rand_r(&seed[phil_id]) / (RAND_MAX + 1.0)) >= thinking_to_eating);
}

static int servings = SERVINGS;

static void* philosodine(void* arg){
  intptr_t phil_id;

  phil_id = (intptr_t) arg;

  seed[phil_id] = phil_id+1;

  while(0 < __sync_fetch_and_sub(&servings, 1L)){
    /* Philosopher was just served */

    /* Picks up his chopsticks */
    pickup_chopsticks(phil_id);

    /* Eats */
    eat(phil_id);

    /* Puts down his chopsticks */
    putdown_chopsticks(phil_id);

    /* And then thinks. */
    think(phil_id);
  }

  return NULL;
}

int main(){
  long i;
  pthread_t phil_threads[5];

  chopsticks_init();  

  for(i = 0; i < 5; i++)
    pthread_create(&phil_threads[i], NULL, philosodine, (void*) i);

  for(i = 0; i < 5; i++)
    pthread_join(phil_threads[i], NULL);    
  
  chopsticks_destroy();

  for(i = 0; i < 5; i++)
    printf("Philosopher %ld ate %d meals.\n", i, count_meals_eaten(i));

  return 0;
}