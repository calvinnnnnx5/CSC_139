//Konstantin Polyashenko
//Programming Assignment 2
//CSC 139

//testing
//make sure it passes all 4 situations
//incorrect number of input, say, 10 or 17 integers.
//incorrect type of input, say, -3a4 in command line.
//16 integers, say, 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
//16 integers, say, -16 +15 14 13 -12 -11 +10 -9 +8 +7 6 +5 4 3 2 1

//standard buffered input/output
#include <stdio.h> 
//data types
#include <sys/types.h>
//declarations for waiting
#include <sys/wait.h>
//standard symbolic constants and types, needed for fork()
#include <unistd.h>
//definitions for vector I/O operations
#include <sys/uio.h>
//string operations
#include <string.h>
//functions for testing and mapping characters
#include <ctype.h>
//PThread library functions
#include <pthread.h> 
//defines various functions for performing general functions
#include <stdlib.h>

int a[8];
int b[8];
int c[4];
//5 threads of type PThread
pthread_t mc[4]; 
pthread_t display;
//variables to hold calculations
//then sent to display process 
pthread_mutex_t mc_lock[4] = 
{
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER
};

//thread function for matrix calculating
void *thr_mc(void *arg)
{	
	printf("MC%d: tid %u\n", (int)arg, (unsigned int)pthread_self());
	
	c[(int)arg] = 0;
	
	int i;
	for (i = 0; i < 4; i++)
	{
		c[(int)arg] += a[i + 4 * ((int)arg / 2)] * b[i * 2 + (int)arg % 2];
	}
	
	//write exiting
	//unlock to not show results 
	printf("MC%d: exiting\n", (int)arg);
	pthread_mutex_unlock(&mc_lock[(int)arg]);

	return 0;
}

//show the other four 
//arg should be null.
void *thr_display(void *arg)
{
	int i;
	
	printf("Display: tid - %u\n", (unsigned int)pthread_self());
	
	//blocked by other threads
	for (i = 0; i < 4; i++)
	{
		pthread_mutex_lock(&mc_lock[i]);
	}

	printf("\nThe first row of the resulting matrix is: %d\t%d\n", c[0], c[1]);
	printf("\nThe second row of the resulting matrix is: %d\t%d\n\n", c[2], c[3]);
	
	//loop to unlock
	for (i = 0; i < 4; i++)
	{
		pthread_mutex_unlock(&mc_lock[i]);
	}
		
	printf("Exiting\n");

	return 0;
}

int main (int argc, char *argv[])
{	
	//check if entered values are correct
	int booleanprint = 0; //value changes if input is not acceptable
	//check if user input contains only ints
	int i;
	int j;
	for (i = 1; i < argc; i++)
    {
        j = 0;
        //positive and negative ints are valid inputs
        //if + or - is present incriment j
        if (argv[i][j] == '+')
        {
            j++;
        }
        else if (argv[i][j] == '-')
        {
        	j++;
        }
        
        for (; argv[i][j] != 0; j++)
        {
            if (!isdigit(argv[i][j]))
            {
                booleanprint = 1;
            }
        }
    }

    //check if user input is acceptable
    if (argc != 17)
    {
        booleanprint = 2;
    }

    //if improper input was detected
    //determine what input error was, notify user and exit
    if (booleanprint != 0)
    {
        if (booleanprint == 1)
        {
            printf("Incorrect type of input.\n");
        }
        else if (booleanprint == 2)
        {
            printf("Incorrect number of input.\n");
        }
        exit(0);
    }
    
    //get values from a and b 
    for (i = 0; i < 8; i++)
    {
        a[i] = atoi(argv[i + 1]);

        b[i] = atoi(argv[i + 9]);
    }
    
	//loop to make the 4 mc threads
	for (i = 0; i < 4; i++)
	{
		printf("Thread Created MC%d\n", i);
		
		pthread_create(&mc[i], NULL, thr_mc, (void *) i);
	}
	
	sleep(1); //waiting to make the display
	
	printf("Display created by thread\n");

	pthread_create(&display, NULL, thr_display, (void *) NULL);
	
	printf("Waiting for threads\n");
	for (i = 0; i < 4; i++)
	{
		pthread_join(mc[i], NULL);
		printf("Waited for MC%d\n", i);
	}
	
	pthread_join(display, NULL);
	printf("Waiting for Display\n");
	
	printf("Finished\n");
    exit(0);
}