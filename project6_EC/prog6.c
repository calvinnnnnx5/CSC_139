//Konstantin Polyashenko
//Programming Assignment 6
//CSC 139

//main C file

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

typedef struct {
	long msg_to;
	long msg_fm;
	char buffer[BUFSIZ]
} MESSAGE;

int     mid;
key_t   key;
struct  msqid_ds buf;
MESSAGE msg;


int main(int argc, char *argv[])
{
	key_t key = ftok(".", 'z');
	if (key == -1)
	{
		perror("Key creation failed.\n");
		exit(0);
	}

	int mid = msgget(key, IPC_CREAT|0660);
	if (mid == -1)
	{
		perror("Message creation failed.\n");
		exit(0);
	}

	ssize_t size;

	while ((size = msgrcv(mid, &msg, sizeof(msg), SERVER, 0) > 0))
	{
		int i;
		for (i = 0; i < size; i++)
		{
			if (msg->buffer[i] >= 'a' && msg->buffer[i] <= 'z')
				msg->buffer[i] += 'A' - 'a'; // Signed chars are great.
		}

		msg->msg_to = msg->msg_fm;
		msg->msg_fm = SERVER;

		msgsnd(mid, &msg, sizeof(msg.buffer), 0);
	}
}
