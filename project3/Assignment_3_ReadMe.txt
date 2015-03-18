Assignment 3 ReadMe

Objective:
- write a multithreaded C program for a bounded-buffer producer/consumer application
  (file copying)
- semaphores must be used to enforce mutually exclusive access to the buffer 
  and to sync producer and consumer with regards to buffer slots/items
- program runs from two C files infile & outfile
	- otherwise print message "Correct Usage: prog3 infile outfile"

Steps of Execution:
- main thread verifies the input, opens infile and creates the outfile
- the main thread spawns a producer thread and a consumer thread
	- waits for both produer and consumer threads to finish before it terminates
- producer and consumer share a buffer of 9 slots(each slot having size of 18 bytes)
- producer reads a string of 18-byte at a time from the infile and places it next
  to available buffer slot
- consumer takes the next available string from a buffer slot and writes it into outfile
- outfile should be a verbatum copy of the infile
- buffer can only be accessed in a mutually exclusive fashion
	- enforced with semaphore vars buf_lock and sem_wait, sem_post operations
- when buffer is full, producer waits until a buffer slot becomes free before it places 
  a string into it
- when buffer is empty, consumer waits for a string to be made available 
- when slot is filled by producer the consumer is notified of a slot availability
- sync conditions between producer and consumer are facilitated by two additional semaphore
  variables slot_avail and item_avail, and the sem_wait and sem_post operations
- include <semaphore.h>, use sem_t to define buf_lock, slot_avail and item_avail
- use sem_init to initialize variables
	- sem_init(&buf_lock, 0, 1);
	- sem_init(&slot_avail, 0, 9);
	- sem_init(&item_avail, 0, 0);
- if x is a semaphore variable, then we can have sem_wait(&x) and sem_post(&x)
- when compiling on a linux machine use -Lposix4 switch
- data type for the buffer are 
	- #define SLOTSIZE 18
	- #define SLOTCNT 9
	- char buffer[SLOTCNT][SLOTSIZE];
- test program for the infile that contains
	Semaphores are effective tools for critical section solution and
	synchronization of concurrent activities. In this exercise, we will
	use semaphores to implement the bounded-buffer producer/consumer
	approach for a file copying application. Specifically, you need to
	create a producer thread and a consumer thread that work concurrently
	with a buffer of 9 slots. The producer produces (reads) a string of
	18-byte at a time from the input file, i.e., this file, and places
	(writes) the string to the next available slot in the buffer in a
	mutually exclusive manner. On the other hand, the consumer fetches a
	an item (18-byte string) from the next filled buffer slot at a time,
	and consumes it (writes it to the output file). When the process is
	finished, the output file is a verbatim copy of the input file.
	Three semaphores are used to enforce the mutually exclusive access to
	the buffer and to synchronize the activities of the producer and
	the consumer. A final note: the last string processed may not be
	exactly 18-byte long.

Complication and Testing:
- compile with gcc prog3.c -o prog3 -lpthread -Lposix4
- run the following tests
	- prog3 infile
	- ls
	- cat infile
	- prog3 infile outfile
	- cat outfile