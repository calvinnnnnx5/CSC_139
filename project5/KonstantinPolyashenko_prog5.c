//Konstantin Polyashenko
//Programming Assignment 5
//CSC 139

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>

struct stat buf; //info about file
char *mm_file; //file is shared between child processes
int fd; //FILE* cannot be used here

//input string
char data[] = "with the mmap call, a disk file gets mapped into memory through demand\npaging, i.e., a page sized portion of the file is initially read into\nmemory through a page fault. subsequent reads and writes to that portion\nof the file are handled as routine memory accesses. this improves file\nprocessing performance. in this assignment, the parent process memory maps\nthis disk file first, and then creates two child processes that each make\nsome changes to this file. when both child processes are done, changes\nmade in memory are synch'ed to the disk file.\n\0";

//first child method
void firstChild()
{
	//print child process and the content
	printf("Child 1 %d reads: \n %s\n", getpid(), mm_file);

	//loop to check for lower case letters
	int i;
	for (i = 0; i < buf.st_size; i++)
	{
		mm_file[i] = toupper(mm_file[i]);
	}

	//synchs the result of the memory mapped stuff and the actual storage
	msync(0, (size_t) buf.st_size, MS_SYNC);
	//prints the uppercase content
	printf("Child 1 %d reads again: \n %s\n", getpid(), mm_file);
	exit(0); //child process is done
}

//second child method
void secondChild()
{
	sleep(1); //so that child 2 will perform its task after child 1 finishes
	//print child process and the content
	printf("Child 2 %d reads: \n %s\n", getpid(), mm_file);

	//manipulates the string for desired outcome
	char *h[] = {"PAGE SIZED\0", "PAGE-SIZED\0"};
	char *c[] = {"MAPPED\0", "LOADED\0"};

	int i;
	int j;
	for (i = 0; i < buf.st_size; i++)
	{
		for (j = 0; h[0][j] != '\0' && i + j < buf.st_size && h[0][j] == mm_file[i + j]; j++);
		if (h[0][j] == '\0')
		{
			for (j = 0; h[1][j] != '\0'; j++)
			{
				mm_file[i + j] = h[1][j];
			}
		}

		for (j = 0; c[0][j] != '\0' && mm_file[i + j] != '\0' && c[0][j] == mm_file[i + j]; j++);
		if (c[0][j] == '\0')
		{
			for (j = 0; c[1][j] != '\0'; j++)
			{
				mm_file[i + j] = c[1][j];
			}
		}
	}

	//prints the manipulated string
	printf("Child 2 %d reads again: \n %s\n", getpid(), mm_file);
	exit(0); //child process is done
}

//main method
int main(int argc, char *argv[])
{
	if (argc != 2)
	{
  	printf("Correct Usage: prog5 infile\n");
    return 0;
  }

  fd = open(argv[1], O_RDWR | O_TRUNC | O_CREAT, S_IRWXO | S_IRWXG | S_IRWXU);

	//checks to see if a file is found or not
	if (fd < 0)
	{
  	perror("No File Found or unable to open file\n");
    exit(0);
  }

  int i;

	//write to the file
  write(fd, data, strlen(data));

	//takes care of file size
  fstat(fd, &buf); //used to determine the size of the file
  mm_file = mmap(0, (size_t) buf.st_size,
    PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	//check if mm_file fails
  if (mm_file == MAP_FAILED)
	{
  	perror("MMAP creation has failed\n");
    close(fd);
    exit(0);
  }

	int child[2]; //child arrays

	//statements to determine what child method to invoke
	if ((child[0] = fork()) == 0)
	{
		firstChild();
	}
	if ((child[1] = fork()) == 0)
	{
		secondChild();
	}

	//wait for child process
	waitpid(child[0], &i, 0);
  printf("Waited for child %d.\n\n", child[0]);
	waitpid(child[1], &i, 0);
  printf("Waited for child %d.\n\n", child[1]);

  //erase the mapping
  munmap(mm_file, buf.st_size);
	//close the file
	close(fd);

  printf("Finished\n");
	return 0; //all is finished
}
