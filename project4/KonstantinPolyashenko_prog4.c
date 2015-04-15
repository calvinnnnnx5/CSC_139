//Konstantin Polyashenko
//Programming Assignment 4
//CSC 139

#include <stdio.h>
#include <pthread.h>

#define SLOTSIZE 12
#define SLOTCNT  10

char buffer[SLOTCNT][SLOTSIZE] = { 0 };

int start = 0;
int end   = 0;
int size  = 0;

FILE *in, *out;

pthread_t producer, consumer;

pthread_mutex_t buf_lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t empty_slot = PTHREAD_COND_INITIALIZER;
pthread_cond_t avail_item = PTHREAD_COND_INITIALIZER;

void *pro()
{
    char c = 1; // Flag variable

    while (c != EOF)
    {
        pthread_mutex_lock(&buf_lock);   // Is the buffer only mine...

        while(size >= SLOTCNT) // Had to have a better check then "start" and "end"
        {
			// If in here...the buffer is full...wait for the signal it is not full.
            pthread_cond_wait(&empty_slot, &buf_lock);
        }

        int i;

        for (i = 0; i < SLOTSIZE && c != EOF; i++)
        {
            // Read one character at a time
            c = (char)fgetc(in);

            buffer[end][i] = c;
        }

        // No overflow.
        end = (end + 1) % SLOTCNT;

        // Let the consumer know there is stuff...
        size++;

		// This is not the way he showed us...but I like it better because
		//  it allows the other guy a chance to go right after...
		// This was just personal preference and works just as good.

        pthread_mutex_unlock(&buf_lock);   // Allow con to access...

        pthread_cond_signal(&avail_item);  // Let con know it can consume...
    }
}

void *con()
{
    char c = 1; // Flag variable

    while (c != EOF)
    {
        pthread_mutex_lock(&buf_lock);   // Is the buffer only mine...

        while(size == 0)
        {
			// If in here...the buffer is empty...wait for the signal that it has something.
            pthread_cond_wait(&avail_item, &buf_lock);
        }

        int i;

        for (i = 0; i < SLOTSIZE; i++)
        {
            // Read the char by char...
            c = (char)buffer[start][i];

            // We don't put EOF into the file.
            if (c == EOF)
                break;

            putc(c, out);
        }

        // No overflow.
        start = (start + 1) % SLOTCNT;

        // Let the producer know there is space...
        size--;

		// This is not the way he showed us...but I like it better because
		//  it allows the other guy a chance to go right after...
		// This was just personal preference and works just as good.

        pthread_mutex_unlock(&buf_lock);   // Allow pro to access...

        pthread_cond_signal(&empty_slot);  // Let the pro know there is more...
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

    // Create/Clear out the outfile.
    out = fopen(argv[2], "w");

    // Let it be known there is an empty slot...
    pthread_cond_signal(&avail_item);

    // Create threads...
    pthread_create(&producer, NULL, pro, (void *)NULL);

    pthread_create(&consumer, NULL, con, (void *)NULL);

    // For each thread and close their file...
    pthread_join(producer, NULL);

    fclose(in);

    pthread_join(consumer, NULL);

    fclose(out);

    pthread_cond_destroy(&avail_item);
    pthread_cond_destroy(&empty_slot);
    pthread_mutex_destroy(&buf_lock);

    // Confirm job is done.
    printf("Done.\n");

    return 0;
}
