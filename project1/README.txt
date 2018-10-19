project 1 ReadMe

//need to know
- processes are primative units for allocating system resources
- each process has its own address space and one thread to control
- a process executes a program, multiple processes can execute 1 program
  but each process has its own copy
- each process has a parent process (parent arranged to create it)
	- each parent creates child processes
	- a child inherits many of the parents attributes

- fork() is a function that creates processes
	- if fork() worked then it returns value of 0 in child process and child process ID in parent process
	- if fork() fails then it returns -1

- pipe() creates a pipe, creates both the reading and writing ends of the pipe
	- a pipe should not be used for a process to talk to itself
	- use pipe to communicate between parent or child, or between siblings
	
- remove() removes a file or directory that is passed in
