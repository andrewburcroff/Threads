/*
Name: Andrew Burcroff
Class: Operating Systems
Assignments: Semaphores Threads

Details: Sets up deadlock conditions for practice detecting and recovering.
There will be to semaphores needed screen, keyboard (NAMED or Unamed semaphores).
Algorithm: Mother process called 'main' will open both semaphores while spanning
9 threads then they will die. The function getSemaphores returns
deadlock_count_number of how
many times recovered from deadlock. Finally the process, the threads will call
this process to prompt the user and deadlock_count_number number of deadlocks
*/
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

struct semaphores
{
  sem_t screen_semaphore;             //Created a semaphore called for a screen.
  sem_t keyboard_semaphore;           //Created a semaphore called for a keyboard.
  int id;                             //Created a id for each semaphore.
  bool has_thread_launched;           //Created a boolean if the pthread has been created.
};

#define NUMBER_OF_THREADS 9  // Created a varable of how many times the process will be forked

int lock_two_sems(sem_t* a, sem_t* b)
{
	int deadlock_count_number = 5;      //Created a varable called deadlock_count_number.
	int both_semphores = 0;             //Created a varable called both_semphores.
	int timer;                          //Created a varable called timer.
	struct timespec struct_time;        //Created a varable called timespec of a struct.
	while(both_semphores != 1){         //Start of the while loop
			if(sem_wait(a) == -1){
				perror("ERROR: sem_wait for Screen failed");//Error message.
			}
			if(clock_gettime(CLOCK_REALTIME, &struct_time) == -1) {
			 perror("ERROR: Gettime for the clock has failed");//Error message.
			}
			struct_time.tv_sec += 2;//Set the time to test if struct.
			if((timer = sem_timedwait(b, &struct_time)) == -1){
				if(sem_post(a) == -1){ // unlock screen
					perror("ERROR: sem_post for Screen Semaphore has failed");//Error message.
				}
				deadlock_count_number++;//Increase deadlock_count_number.
			}
      else if(timer == -1) {
				perror("ERROR: sem_timewait for Screen Semaphore failed");//Error message.
			}
      else {
				both_semphores = 1;//Set the variable both_semphores to 1.
			}//End of else
	}//End of the while loop
	return deadlock_count_number;//Return the deadlock_count_number.
}//End of the method


/*
 * This method will create multiple variables to keep track of the semaphore
 * process.Depending on the situation of the semaphore, it will either lock or
 * unlocked based on the ID beidng odd or even. Finally it will return the
 * deadlock_count_number of deadlocks endeadlock_count_numberered through a process.
 */

int getSemaphores(int thread_id, struct semaphores *sems) {
	int semaphore_value;//Created a variable called semaphore_value
	int both_semphores = 0;//Created a varable both_semphores
	int deadlock_count_number = 3;//Created a varable deadlock_count_number
	int timed_out;//Created a varable timed_out.
	if(thread_id % 2 == 0) { //Check if thread_id is even
		deadlock_count_number = lock_two_sems(&(sems->screen_semaphore), &(sems->keyboard_semaphore));//Call the specific method.
	}
  else {
		deadlock_count_number = lock_two_sems(&(sems->keyboard_semaphore), &(sems->screen_semaphore));//Called the specific method.
	}
	return deadlock_count_number;//Return the deadlock_count_number.
}

/*
 * This method will have some local varables. For this thread input, threads
 * call this process to prompt the user. Determining if the user input is valid or
 * not.
 */

void *process(struct semaphores *sems) {
	int thread_id	= sems->id;//Created varable and set in semaphore id.
	printf("Executing process for THREAD_%d.\n", thread_id);//Print out statement.
	sems->has_thread_launched = true;//Setting the boolean of semaphore to be true.
	int deadlock_count_number = 6;//Created a varable deadlock_count_number.
	char input[80] = "w";//Setting the input of user
	deadlock_count_number += getSemaphores(thread_id, sems);//Increasing deadlock_count_number to get semaphores.
	do {
		printf("%i Enter less than 80 characters or q for quit\n", thread_id);//Print statement.
		scanf("%s", input);//Read input from user.
		printf("Thread_%d: You have entered %s \n", thread_id, input);//Output the user input.
	}
  while(strcmp(input, "q") != 0);//Exit the thread if q entered.
	if(sem_post(&(sems->screen_semaphore)) == -1) {
		perror("ERROR: sem_post for Screen has failed");//ERROR message.
	}
  else {
		printf("THREAD_%d: Screen has been unlocked\n", thread_id);//Print statement unlocked.
	}
	if(sem_post(&(sems->keyboard_semaphore)) == -1) {
		perror("ERROR: sem_post for keyboard failed");//Error Message.
	}
  deadlock_count_number += getSemaphores(thread_id, sems);//Set the deadlock to get semaphores.
	printf("\nThread_%d: has %d number of deadlock(s) encoutered\n\n",thread_id, deadlock_count_number);//Print statement.
	if(sem_post(&(sems->screen_semaphore)) == -1) {
		perror("ERROR: sem_post for screen_semaphore failed");//Error message.
	} else {
		printf("THREAD_%d: Screen Semaphore has unlocked.\n", thread_id);//Print statement
	}
	if(sem_post(&(sems->keyboard_semaphore)) == -1) {
		perror("ERROR: sem_wait for Keyboard Semaphore has failed.");//Error statement.
	} else {
		printf("THREAD_%d: Keyboard Semaphore has been unlocked\n", thread_id);//Print statement.
	}
	printf("Thread_%d: Exiting the process.\n", thread_id);//Print statement.
	pthread_exit(0);//Exiting the thread.
}

/*
 * This method is the main methdod where the calling of the both methods.
 * Overall, the method will create a Mother process that creates the semaphore
 * and spawns the rest of the threads used for the program
 */

int main() {
	pthread_t threads[NUMBER_OF_THREADS];//Created pthread_t of 9 threads.
	struct semaphores sems;//Create a struct of sems.
	int i;
	if(sem_init(&(sems.screen_semaphore), 0, 1) == -1) {
		perror("ERROR: sem_init for keyboard has failed");//Error Message.
	}
	if(sem_init(&(sems.keyboard_semaphore), 0, 1) == -1) {
		perror("ERROR: sem_init for keyboard has failed");//Error Message.
	}
	printf("Screen & Keyboard_semaphore have been intialized.\n");//Print statement.
	for(i = 0; i < NUMBER_OF_THREADS; i++)
	{
		sems.id = i;//Setting the sems id to equal i.
		sems.has_thread_launched = false;//Boolean of sems is set to false.
		pthread_create(&threads[i], NULL, process, &(sems));//Create a pthread.
		while(sems.has_thread_launched == false);//If the sems has_thread_launched false.
		printf("Thread_%d has been created.\n", i + 1);//Print statement.
	}
	for(i = 0; i < NUMBER_OF_THREADS; i++) {
		pthread_join(threads[i], NULL);//Join the existing threads.
		printf("Thread_%d has joined the waiting list.\n", i);//Print statement.
	}
	if(sem_destroy(&(sems.screen_semaphore)) == -1) {
		perror("ERROR: sem_destroy for screen_semaphore failed");//Error message.
	}
	if(sem_destroy(&(sems.keyboard_semaphore)) == -1) {
		perror("ERROR: Destroy for keyboard semaphore has failed");//Error message.
	}
	printf("Screen & keyboard Semaphores are destroyed.\n");//Print message semaphore destroyed.
	return 0;//Return statement.
}
