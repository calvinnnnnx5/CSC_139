// Konstantin Polyashenko
// Programming Assignment 4
// CSC 139

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/uio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

//18 bytes read in at a time from infile
#define SLOTSIZE 18
//9 total slots with 18
#define SLOTCNT  9
//initiate first column in first row to 0
//all other items are also initilized to 0
char buffer[SLOTCNT][SLOTSIZE] = { 0 };

int start = 0;
int end   = 0;
int size  = 0;

FILE *in;
FILE *out;

pthread_t producer;
pthread_t consumer;

pthread_mutex_t buf_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty_slot = PTHREAD_COND_INITIALIZER;
pthread_cond_t item_avail = PTHREAD_COND_INITIALIZER;

void *pro()
{
  char c = 1; //flag variable

  while (c != EOF)
  {
    pthread_mutex_lock(&buf_lock); //checks the buffer to see if all is proper

    while(size >= SLOTCNT)
    {
      //either buffer is full or must wait for signal
      pthread_cond_wait(&empty_slot, &buf_lock);
    }

    int i;

    //loop to read in characters
    for (i = 0; i < SLOTSIZE && c != EOF; i++)
    {
      c = (char)fgetc(in);
      buffer[end][i] = c;
    }

    end = (end + 1) % SLOTCNT;
    size++;

    pthread_cond_signal(&item_avail);
    pthread_mutex_unlock(&buf_lock);
  }
}

void *con()
{
  // our extra variables, as stated in PDf
  // again to parse through, I created a simple variable
  // flag
  char c = 1;

  // while we arent at the end of the file
  while (c != EOF)  {

  // same as pro thread
  pthread_mutex_lock(&buf_lock);

  // same as pro thread
  while(size == 0)
  {
    pthread_cond_wait(&item_avail, &buf_lock);
  }

  int i; //flag variable

  //reading in the characters
  for (i = 0; i < SLOTSIZE; i++)
  {
    c = (char)buffer[start][i];

    //statenment checks for EOF
    if (c == EOF)
    {
      break;
    }
    putc(c, out);
  }

  start = (start + 1) % SLOTCNT;

  size--;

  pthread_cond_signal(&empty_slot);
  pthread_mutex_unlock(&buf_lock);
  }
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("File was used properly\n");
    return 0;
  }

  //checks if the file is good
  in = fopen(argv[1], "r");

  if (in == 0)
  {
    printf("Problem with input file is detected\n");
    return 0;
  }

  //handles the outfile
  out = fopen(argv[2], "w");

  pthread_cond_signal(&item_avail);
  pthread_create(&producer, NULL, pro, (void *)NULL);
  pthread_create(&consumer, NULL, con, (void *)NULL);
  pthread_join(producer, NULL);
  fclose(in);
  pthread_join(consumer, NULL);
  fclose(out);

  pthread_cond_destroy(&item_avail);
  pthread_cond_destroy(&empty_slot);

  pthread_mutex_destroy(&buf_lock);

  printf("All actions have been completed\n");

  return 0;
}
