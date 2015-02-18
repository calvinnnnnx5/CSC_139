//Konstantin Polyashenko
//Programming Assignment 1
//CSC 139

//testing
//make sure it passes all 4 situations
//incorrect number of input, say, 10 or 17 integers.
//incorrect type of input, say, -3a4 in command line.
//16 integers, say, 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
//16 integers, say, 12 –11 10 –9 +8 7 +6 -5 -4 +3 2 1 2 3 4 5

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

/*multiprocessed C program to do matric multiplication 
of two integer matrices, and print the results*/

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

    int mc[5]; //MC0-MC3 and display
    int pipes[4][2]; //needed pipes
    char intothePipe[16]; //intermediate String storage 

    //make 4 pipes
    for (i = 0; i < 4; i++)
    {
        pipe(pipes[i]);
    }

    printf("Forking children.\n");

    //fork 5 processes
    for (j = 0; j < 5 && (mc[j] = fork()) != 0; j++)
    {
        if (mc[j] == -1)
        {
            perror("fork");
            exit(0);
        }
    }

    //finds child processes
    //if 
    if (j < 5)
    {
        //getpid() returns process ID of the calling process
        //getppid() returns the process ID of the parent calling the function
        printf("%d\t%d\n", getpid(), getppid());
    }

    //4 children than calculate values
    if (j < 4)
    {
        int c = 0;

        for (i = 0; i < 4; i++)
        {
            close(pipes[i][0]);

            if (i == j)
            {
                continue;
            }

            close(pipes[i][1]);
        }

        int aval_holder;
        int bval_holder;
        for (i = 0; i < 1; i++)
        {
            if (j == 0)
            {
                c = (a[0]*b[0])+(a[1]*b[2])+(a[2]*b[4])+(a[3]*b[6]);
            }
            else if (j == 1)
            {
                c = (a[0]*b[1])+(a[1]*b[3])+(a[2]*b[5])+(a[3]*b[7]);
            }
            else if (j == 2)
            {
                c = (a[4]*b[0])+(a[5]*b[2])+(a[6]*b[4])+(a[7]*b[6]);
            }
            else if (j == 3)
            {
                c = (a[4]*b[1])+(a[5]*b[3])+(a[6]*b[5])+(a[7]*b[7]);
            }
        }

        //sends formatted output to string pointed to by string
        sprintf(intothePipe, "%d", c);
        
        write(pipes[j][1], &intothePipe, 16);
        
        close(pipes[j][1]);

        printf("Child %d has been calculated.\n", getpid());
        printf("%d sent.\n", c);
    }

    //detects fifth child
    //this is the display child
    else if (j == 4)
    {
        int c[4];

        for (i = 0; i < 4; i++)
        {
            //data is being sent
            //writing pipe closure
            close(pipes[i][1]);

            //buffer is cleared
            sprintf(intothePipe, "%d", 0);

            read(pipes[i][0], &intothePipe, 16);

            //take i index of c string and converts to int
            c[i] = atoi(intothePipe);

            //close read pipe
            close(pipes[i][0]);
        }

        printf("\nThe first row of the resulting matrix is:%d\t%d\n", c[0], c[1]);
        printf("\nThe second row of the resulting matrix is:%d\t%d\n\n", c[2], c[3]);
    }

    else 
    {
        printf("Waiting for children.\n");

        int status = 0;

        for (i = 0; i < 5; i++)
        {
            //parent waits until all 5 children finish their tasks
            waitpid(mc[i], &status, 0);

            if (i < 4)
            {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }
        }
    }
    //if less than 5, indicates child
    //if not then its parent
    if (j < 5)
    {
        printf("Child is terminated.\n");
    }
    else 
    {
        printf("Parent is terminated.\n");
    }
    exit(0);

}

