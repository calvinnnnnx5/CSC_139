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
	// as stated in PDF, program only runs with 2 file names in command line
	// otherwise the message is printed, then it terminates
	if (argc != 3)
	{
  	printf("Correct Usage: client1 infile1 infile2\n");
    return 0;
  }


	// The IPCS key is created in given directory
	// from PDF
	key_t key = ftok(".", 'z');
	if (key == -1)
	{
		perror("Key could not be made\n");
		exit(0);
	}

	// Message queue is created, "derived from key value"
	// from PDF
	int mid = msgget(key, IPC_CREAT|0660);
	if (mid == -1)
	{
		perror("Message queue could not be made\n");
		exit(0);
	}

  // variable for stepping through
	int i = 1;
	int fd;

	while (i < 3)
	{
		// the file is read into
		fd = open(argv[i], O_RDONLY, S_IRWXO | S_IRWXG | S_IRWXU);
		// what has been read in placed in buffer
		read(fd, msg.buffer, BUFSIZ);

		// print what has been read before its manipulated
    printf("%s\n", msg.buffer);

		//Setup message to be sent to Server
		msg.msg_to = SERVER;
    // use the ID for linking
		msg.msg_fm = getpid();

		// Send message, we dont use "sizeof" as stated
		// in PDF due to problems it causes, instead
		// buffer is used
		msgsnd(mid, &msg, sizeof(msg.buffer), 0);
		// Receive the now updated message from Server, use ID
		// to get specific message
		msgrcv(mid, &msg, sizeof(msg), getpid(), 0);

		// Print the message, should be uppercase
		printf("%s\n", msg.buffer);

		// end file
		close(fd);
		i++;
	}
	printf("Client 1 has finished\n");
}
