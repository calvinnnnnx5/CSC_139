//Konstantin Polyashenko
//Programming Assignment 3
//CSC 139

//compiling
//gcc KonstantinPolyashenko_prog3.c -o prog3 -lpthread -Lposix4

//testing
//prog3 infile
//ls
//cat infile
//prog3 infile outfile
//cat outfile

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/uio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

int start;
int end;

FILE *in; 
FILE *out;

pthread_t producer; 
pthread_t consumer;

sem_t buf_lock;
sem_t slot_avail;
sem_t item_avail;

//18 bytes read in at a time from infile
#define SLOTSIZE 18 
//9 total slots with 18 
#define SLOTCNT 9
//initiate first column in first row to 0
//all other items are also initilized to 0
char buffer[SLOTSIZE][SLOTCNT] = {0};

int main(int argc, char *argv[])
{
    //checks if input is proper, the infile
    if (argc != 3)
    {
        printf("Correct Usage: prog3 infile outfile");
        return 0;
    }
    
    //file is opened
    //check if file is valid
    in = fopen(argv[1], "r");
    
    if (in == 0)
    {
        printf("Infile cannot be found.\n");
        return 0;
    }
    
    //create the output file
    //clear the output file
    out = fopen(argv[2], "w");
    
    if (sem_init(&buf_lock, 0, 1))
    {
        printf("Error detected during 'buf_lock' creation");
    }
    else if (sem_init(&slot_avail, 0, 9))
    {
        sem_destroy(&buf_lock);
        printf("Error detected during 'slot_avail' creation");
    }
    else if (sem_init(&item_avail, 0, 0))
    {
        sem_destroy(&buf_lock);
        sem_destroy(&slot_avail);
        printf("Error detected during 'item_avail' creation");
    }
    
    //thread creation
    pthread_create(&producer, NULL, pro, (void *)NULL); 
    pthread_create(&consumer, NULL, con, (void *)NULL);

    pthread_join(producer, NULL);
    fclose(in);
    pthread_join(consumer, NULL);
    fclose(out);
    
    //everything has been done
    //program closes
    printf("Done.\n");
    
    //all functions should have a return
    //return of 0 to identify proper execution
    return 0;
}

void *pro()
{
    char c = 1; //flag variable
    
    while (c != EOF)
    {
        sem_wait(&slot_avail); //check if proper space is available
        
        sem_wait(&buf_lock); //does the buffer belong to anybody else
        
        int i; //i var, for for loop
        //loop to read in single char at a time
        for (i = 0; i < SLOTCNT && c != EOF; i++)
        {
            c = (char)fgetc(in);       
            buffer[end][i] = c;
        }
        
        //no overflow
        end = (end + 1) % SLOTSIZE;
        
        sem_post(&buf_lock);
        sem_post(&item_avail); 
    }
}

void *con()
{
    char c = 1; //flag var
    
    while (c != EOF)
    {
        //determines that items are available
        sem_wait(&item_avail);
        //determines if buffer belongs to somebody else
        sem_wait(&buf_lock); 
        
        int i;
        //loop to read in char by char
        for (i = 0; i < SLOTCNT; i++)
        {
            c = (char)buffer[start][i];
            
            if (c == EOF)
            {
                break;
            }
            putc(c, out);
        }
        
        //no overflow
        start = (start + 1) % SLOTSIZE;
        
        sem_post(&buf_lock);      
        sem_post(&slot_avail); 
    }
}