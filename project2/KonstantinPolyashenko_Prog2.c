//Konstantin Polyashenko
//Programming Assignment 1
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

// macromacro will not work with mc_lock[number] = PTHREAD_MUTEX_INITIALIZER;
// macro malfunctions with an array
int a[8];
int b[8];
int c[4];
// 5 threads of type PThread
pthread_t mc[4]; 
pthread_t display;
pthread_mutex_t mc_lock[4] = 
{
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER
};

// thread function for matrix calculating
void *thr_mc(void *arg)
{
	int i;
	
	printf("MC%d: tid - %u\n", (int)arg, (unsigned int)pthread_self());
	
	//sleep(1);
	
	c[(int)arg] = 0;
	
	for (i = 0; i < 4; i++)
	{
		c[(int)arg] += a[i + 4 * ((int)arg / 2)] * b[i * 2 + (int)arg % 2];
	}
	
	// write exiting
	// unlock to not show results 
	printf("MC%d: exiting\n", (int)arg);
	pthread_mutex_unlock(&mc_lock[(int)arg]);
}

// Display the other 4 matrices.
// arg should be null.
void *thr_display(void *arg)
{
	int i;
	
	printf("Display: tid - %u\n", (unsigned int)pthread_self());
	
	// Be blocked by the other threads...
	for (i = 0; i < 4; i++)
	{
		pthread_mutex_lock(&mc_lock[i]);
	}
	
	printf("\nThe first row of the resulting matrix is:\t%d\t%d\n", c[0], c[1]);
	printf("\nThe second row of the resulting matrix is:\t%d\t%d\n\n", c[2], c[3]);
	
	// Unlock the recently locked mutexes.
	for (i = 0; i < 4; i++)
	{
		pthread_mutex_unlock(&mc_lock[i]);
	}
		
	printf("Display: exiting\n");
}

int main (int argc, char *argv[])
{
    // Verify the input.
    if (argc != 17)
    {
        printf("Incorrect number of inputs. Use 16 integers. %d\n", argc - 1);
        exit(0);
    }
    
    int i, j;
    
    //printf("Input validating....\n");
    
    for (i = 1; i < argc; i++)
    {
        j = 0;
        
        // Allow '+' or '-'
        if (argv[i][j] == '-' || argv[i][j] == '+')
        {
            j++;
        }
        
        for (; argv[i][j] != 0; j++)
        {
            //printf("hey%s", argv[i][j]);
            if (!isdigit(argv[i][j]))
            {
                printf("Invalid input. Use 16 integers. %s is invalid.\n", argv[i]);
                exit(0);
            }
        }
    }
    
    //printf("Input valid. Filling arrays.\n");
    
    // Get the values for A & B.
    for (i = 0; i < 8; i++)
    {
        //printf("a[%d]", i);
        a[i] = atoi(argv[i + 1]);
        //printf("b[%d]\n", i);
        b[i] = atoi(argv[i + 9]);
    }
    
	// Create the 4 mc threads with a for loop.
	for (i = 0; i < 4; i++)
	{
		printf("Main: creating thread 'MC%d'\n", i);
		
		// -- DEPRECIATED --
		// Lock the mutex for each mc thread
		// to avoid the racing condition of the display thread
		// happening to run before mc0.
		//pthread_mutex_lock(&mc_lock[i]);
		
		pthread_create(&mc[i], NULL, thr_mc, (void *) i);
	}
	
	sleep(1); // To avoid the racing condition...wait to create display.
	
	printf("Main: creating thread 'Display'\n");
	// Display didn't get in the for loop.
	pthread_create(&display, NULL, thr_display, (void *) NULL);
	
	// The join is similar to creation, with 'join' instead of create.
	printf("Main: waiting for threads\n");
	for (i = 0; i < 4; i++)
	{
		pthread_join(mc[i], NULL);
		printf("Main: waited for 'MC%d'\n", i);
	}
	
	pthread_join(display, NULL);
	printf("Main: waited for 'Display'\n");
	
	// Exit the program.
	printf("Main: finished\n");
    exit(0);
}