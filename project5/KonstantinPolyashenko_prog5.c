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

int fd; FILE* //cant be used for map

char data[] = "memory mapping a disk file into virtual memory, done via the mmap call,
							\nallows file i/o to be treated as routine memory accesses.\nin this exercise,
							this file gets memory mapped first. Then two child\nprocesses: child-1 and child-2,
							 each will make some changes to the file.\n\0";

void upchild()
{
	printf("Child 1 %d reads: \n %s\n", getpid(), mm_file);
	int i;
	for (i = 0; i < buf.st_size; i++)
	{
		if (mm_file[i] >= 'a' && mm_file[i] <= 'z')
		{
			mm_file[i] += 'A' - 'a';
		}
	}
	msync(0, (size_t) buf.st_size, MS_SYNC);
	printf("Child 1 %d reads again: \n %s\n", getpid(), mm_file);

	exit(0); //exit child process
}

void repchild()
{
	sleep(1);
	printf("Child 2 %d reads: \n %s\n", getpid(), mm_file);

	char *h[] = {"MEMORY MAPPING\0", "MEMORY-MAPPING\0"};
	char *c[] = {"CHANGES\0", "UPDATES\0"};

	int i, j;
	for (i = 0; i < buf.st_size; i++)
	{
		// Are there functions in "string.h" to do this...yes...
		// Did I want to look at man pages or have fun...I wanted fun...
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
	printf("Child 2 %d reads again: \n %s\n", getpid(), mm_file);

	exit(0); //exit child process
}

int main(int argc, char *argv[])
{
	//check for correct input
  if (argc != 2)
  {
  	printf("Correct Usage: prog5 filename");
    return 0;
  }

	//create then open file
	//give read/write access to users
	fd = open(argv[1], O_RDWR | O_TRUNC | O_CREAT, S_IRWXO | S_IRWXG | S_IRWXU);

  if (fd < 0)
  {
		perror("Error creating file.\n");
    exit(0);
  }

  int i;

  write(fd, data, strlen(data)); //write file

  fstat(fd, &buf); //information about file

	//creates a pointer
  mm_file = mmap(0, (size_t) buf.st_size,
		PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	//MAP_FAILED is returned when there is a failure
  if (mm_file == MAP_FAILED)
  {
  	perror("mmap call fails\n");
    close(fd);
    exit(0);
  }

	//children in arrays
	int child[2];
	if ((child[0] = fork()) == 0)
	{
		upchild();
	}
	if ((child[1] = fork()) == 0)
	{
		repchild();
	}

	//wait for the children
	waitpid(child[0], &i, 0);
  printf("Waited for child %d.\n\n", child[0]);
	waitpid(child[1], &i, 0);
  printf("Waited for child %d.\n\n", child[1]);

  //mapping deleted
  munmap(mm_file, buf.st_size);
	close(fd);

  printf("Done.\n"); //all has finished

	return 0;
}
