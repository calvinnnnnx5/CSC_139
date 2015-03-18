//Konstantin Polyashenko
//Programming Assignment 3
//CSC 139

//compiling
//gcc prog3.c -o prog3 -lpthread -Lposix4

//testing
//prog3 infile
//ls
//cat infile
//prog3 infile outfile
//cat outfile

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define SLOTSIZE 12
#define SLOTCNT  10

char buffer[SLOTSIZE][SLOTCNT] = { 0 };

int start, end;

FILE *in, *out;

pthread_t producer, consumer;

sem_t buf_lock;
sem_t slot_avail;
sem_t item_avail;

void *pro()
{
    char c = 1; //flag variable
    
    while (c != EOF)
    {
        sem_wait(&slot_avail); //check if proper space is available
        
        sem_wait(&buf_lock); //does the buffer belong to anybody else
        
        int i;
        
        for (i = 0; i < SLOTCNT && c != EOF; i++)
        {
            c = (char)fgetc(in); //read in one char at a time          
            buffer[end][i] = c;
        }
        
        //no overflow
        end = (end + 1) % SLOTSIZE;
        
        sem_post(&buf_lock);   // Allow con to access...
        
        sem_post(&item_avail); // Let con know it can consume...
    }
}

void *con()
{
    char c = 1; // Flag variable
    
    while (c != EOF)
    {
        sem_wait(&item_avail); // Are items available...
        
        sem_wait(&buf_lock);   // Is the buffer only mine...
        
        int i;
        
        for (i = 0; i < SLOTCNT; i++)
        {
            // Read the char by char...
            c = (char)buffer[start][i];
            
            // We don't put EOF into the file.
            if (c == EOF)
                break;
            
            putc(c, out);
        }
        
        // No overflow.
        start = (start + 1) % SLOTSIZE;
        
        sem_post(&buf_lock);   // Allow pro to access...
        
        sem_post(&slot_avail); // Let the pro know there is more...
    }
}

int main(int argc, char *argv[])
{
    // Verify correct input.
    // I am assuming that we can anyfile, not just files named
    // "infile" and "outfile"
    if (argc != 3)
    {
        printf("Correct Usage: prog3 infile outfile");
        return 0;
    }
    
    // Open and check if the files is valid.
    in = fopen(argv[1], "r");
    
    if (in == 0)
    {
        printf("Infile does not exits.\n");
        return 0;
    }
    
    //create the output file
    //clear the output file
    out = fopen(argv[2], "w");
    
    // Safety first...
    if (sem_init(&buf_lock, 0, 1))
    {
        printf("Error creating 'buf_lock'.");
    }
    else if (sem_init(&slot_avail, 0, 10))
    {
        sem_destroy(&buf_lock);
        printf("Error creating 'slot_avail'.");
    }
    else if (sem_init(&item_avail, 0, 0))
    {
        sem_destroy(&buf_lock);
        sem_destroy(&slot_avail);
        printf("Error creating 'item_avail'.");
    }
    
    //make threads
    pthread_create(&producer, NULL, pro, (void *)NULL);
    
    pthread_create(&consumer, NULL, con, (void *)NULL);

    //for each thread close thier file
	pthread_join(producer, NULL);
    
    fclose(in);
    
	pthread_join(consumer, NULL);
    
    fclose(out);
    
    //confirm that everythign was done properly
    printf("Done.\n");
    
    return 0;
}