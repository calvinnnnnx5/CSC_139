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

int mid;
key_t key;
struct msqid_ds buf;
MESSAGE msg;

int main(int argc, char *argv[])
{
	//only 1 file is to be accepted
	if (argc != 2)
	{
  	printf("Correct Usage: client2 infile3\n");
    return 0;
  }

	key_t key = ftok(".", 'z'); //get key
	if (key == -1)
	{
		perror("Key could not be made\n");
		exit(0);
	}

	int mid = msgget(key, IPC_CREAT|0660);
	if (mid == -1)
	{
		perror("Message queue could not be made\n");
		exit(0);
	}

	int fd;
	fd = open(argv[1], O_RDONLY, S_IRWXO | S_IRWXG | S_IRWXU); //file is read in
	read(fd, msg.buffer, BUFSIZ);

	printf("%s\n", msg.buffer);

	msg.msg_to = SERVER; //message is sent to server

	msg.msg_fm = getpid(); //ID is recieved

	msgsnd(mid, &msg, sizeof(msg.buffer), 0); //message is sent
	msgrcv(mid, &msg, sizeof(msg), getpid(), 0); //message is recieved
	printf("%s\n", msg.buffer); //message with capital letters is printed

	close(fd);

	msg.msg_to = SERVER;
	msg.msg_fm = getpid();
	msg.buffer[0] = 0;
	msgsnd(mid, &msg, sizeof(msg.buffer), 0); //message given to server
	printf("Client 2 done\n");
}
