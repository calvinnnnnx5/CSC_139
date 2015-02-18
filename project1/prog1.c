//Konstantin Polyashenko
//Programming Assignment 1
//CSC 139

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
//defines various functions for performing general functions
#include <stdlib.h>

//multiprocessed C program to do matric multiplication 
    //of two integer matrices, and print the results

//main method
//argc is argument count, # of arguments passed to the program
//first argument in argc is a.out, so plus 1
int main(int argc, char *argv[]) 
{
    //verifies if entered values are correct//
    int booleanprint = 0; //value will change if input is not acceptable
    //check if user input contains only ints
    int i;
    int j;
    for (i = 1; i < argc; i++)
    {
        j = 0;
        //positive and negative ints are valid input
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

    //
    int a[8]; //2d array for first 4 values
    int b[8]; //2d array for second 4 values
    //fill arrays with input value
    for (i = 0; i < 8; i++)
    {
        //atoi() function converts string args into integer
        a[i] = atoi(argv[i+1]);
        b[i] = atoi(argv[i+9]);
    }

    int mc[5];
    int pipes[4][2];
    char forPipe[16];

    //make 4 pipes
    for (i = 0; i < 4; i++)
    {
        pipe(pipes[i]);
    }

    //fork 5 processes
    for (j = 0; j < 5 && (mc[j] = fork()) != 0; j++)
    {
        mc[j];
    }

    //finds child processes
    //if 
    if (j < 5)
    {
        //getpid() returns process ID of the calling process
        //getppid() returns the process ID of the parent calling the function
        printf("%d\t%d\n", getpid(), getppid());
    }

    //childen that will make the 4 calculations
    if (j < 4)
    {
        int c = 0;

        for (i = 0; i < 4; i++)
        {
            //
            if (i == j)
            {
                continue;
            }

            close(pipes[i][1]);
        }

        int aval_holder;
        int bval_holder;
        for (i = 0; i < 4; i++)
        {
            //4 * (j / 2) determines the row
            //i determines the column
            aval_holder = a[i + 4 * (j / 2)];
            //i * 2 determines the row
            //j % 2 determines the column
            bval_holder = b[i * 2 + j % 2];
            //c holds the end result
            c = c + aval_holder * bval_holder;
        }

        //sends formatted output to string pointed to by str
        sprintf(forPipe, "%d", c);

        write(pipes[j][1], &forPipe, 16);

        close(pipes[j][1]);

        printf("Child %d calculated/sent %d.\n", getpid(), c);
    }
}

