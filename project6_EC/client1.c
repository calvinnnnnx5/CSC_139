//Konstantin Polyashenko
//Programming Assignment 6
//CSC 139

//C file for the first client

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

#define SERVER 1L

typedef struct
{
	long msg_to;
	long msg_fm;
	char buffer[BUFSIZ];
} MESSAGE;

int mid;
key_t key;
struct msqid_ds buf;
MESSAGE msg;

int main(int argc, char *argv[])
{
	// Input validation.
	if (argc != 3)
  {
  	printf("Correct Usage: client1 infile1 infile2\n");
    return 0;
  }

	// Create the ipcs key based on the current directory
	// to maintain a shared one between files in the same
	// directory.
	key_t key = ftok(".", 'z');
	if (key == -1)
	{
		perror("Key creation failed.\n");
		exit(0);
	}

	// Create the message queue from the key with permissions
	// for the same user and group.
	int mid = msgget(key, IPC_CREAT|0660);
	if (mid == -1)
	{
		perror("Message creation failed.\n");
		exit(0);
	}

	int i = 1;
	int fd;

	while (i < 3)
	{
		// read file from argv
		fd = open(argv[i], O_RDONLY, S_IRWXO | S_IRWXG | S_IRWXU);
		// fill buffer
		read(fd, msg.buffer, BUFSIZ);

		// Prep the message for transport
		msg.msg_to = SERVER;

		msg.msg_fm = getpid(); // It is a good type.

		// Send and then receive.
		msgsnd(mid, &msg, sizeof(msg.buffer), 0);
		// Get the message based on pid.
		msgrcv(mid, &msg, sizeof(msg), getpid(), 0);

		// Print, close file, loop.
		printf("%s\n", msg.buffer);

		close(fd);

		i++;
	}

	printf("Client 1 exiting...\n");
}
