//Konstantin Polyashenko
//Programming Assignment 6
//CSC 139

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
int	mid;
key_t	key;
struct msqid_ds buf;
MESSAGE msg;

int main(int argc, char *argv[])
{
	key_t key = ftok(".", 'z'); //IPCS is made
	if (key == -1)
	{
		perror("Key could not be made\n");
		exit(0);
	}

	//message queue is made
	int mid = msgget(key, IPC_CREAT|0660);
	if (mid == -1)
	{
		perror("Message queue could not be made\n");
		exit(0);
	}

	while (msgrcv(mid, &msg, sizeof(msg), SERVER, 0) != -1)
	{
		//if the queue is equal to 0, termination occurs
		if (strlen(msg.buffer) == 0)
		{
			msgctl(mid, IPC_RMID, (struct msqid_ds *) 0);
			printf("No message recieved\n");
			exit(0);
		}

    //produces uppercased letter message
		int i;
		for (i = 0; i < strlen(msg.buffer); i++)
		{
			msg.buffer[i] =  toupper(msg.buffer[i]);
		}

		//message is sent
		msg.msg_to = msg.msg_fm;
		msg.msg_fm = SERVER;

		printf("%s\n", msg.buffer);
		msgsnd(mid, &msg, sizeof(msg.buffer), 0);
	}
}
