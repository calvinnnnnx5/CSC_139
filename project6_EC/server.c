//Konstantin Polyashenko
//Programming Assignment 6
//CSC 139

//C file for the server

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
}MESSAGE;

int	mid;
key_t	key;
struct	msqid_ds buf;
MESSAGE msg;

int main(int argc, char *argv[])
{
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

	// It will fail if it returns -1.
	while (msgrcv(mid, &msg, sizeof(msg), SERVER, 0) != -1)
	{
		// Make sure the message is not zero...else close the queue.
		if (strlen(msg.buffer) == 0)
		{
			msgctl(mid, IPC_RMID, (struct msqid_ds *) 0);
			printf("Empty message received...closing queue and exiting.\n");
			exit(0);
		}

		// Loop through the length of the message and bring things to upper.
		int i;
		for (i = 0; i < strlen(msg.buffer); i++)
		{
			if (msg.buffer[i] >= 'a' && msg.buffer[i] <= 'z')
				msg.buffer[i] += 'A' - 'a'; // Signed chars are great.
		}

		// Return to sender.
		msg.msg_to = msg.msg_fm;

		msg.msg_fm = SERVER;

		printf("%s\n", msg.buffer);

		msgsnd(mid, &msg, sizeof(msg.buffer), 0);
	}
}
