/* user.c : User processes
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>

/* Your code goes here */

void proc1(void);
void proc2(void);
void proc3(void);
void handler1(void *arg);
void handler2(void *arg);

unsigned char console[80];

/*
* idleproc
*
* @desc:	executes the idle process
*/
void idleproc ()
{
	for(;;);
}


void handler1(void *arg)
{
	sprintf(console, "called handler1");
	sysputs(console);
}

void handler2(void *arg)
{
	sprintf(console, "called handler2");
	sysputs(console);
}

/*
* root
*
* @desc:	executes the root process
*/
void root()
{	
	int rc,fd,n,child_pid[3];
	unsigned int buffer[10];
	unsigned int byte=10;
	void *old_handler = NULL;

	sprintf(console, "Welcome to xeros!");
	sysputs(console);

	fd = sysopen(1);
	sprintf(console, "sysopen[%d]: %d", 1, fd);
	sysputs(console);

	/* read 10 characters from keyboard into user buffer */
	if(fd != -1)	
		sysread(fd, buffer, byte);

	sprintf(console, "sysread[%d]: %s", 1, buffer);
	sysputs(console);
	
	/* attempt to open no-echo keyboard */
	sprintf(console, "rc: %d", sysopen(0));
	sysputs(console);


	/* attempt to open echo keyboard */
	sprintf(console, "rc: %d", sysopen(0));
	sysputs(console);

	/* close echo keyboard */
	if(fd != -1)
	{
		fd = sysclose(fd);
		sprintf(console, "sysclose[%d]: %d", 1, fd);
		sysputs(console);
	}

	sprintf(console, " ");
	sysputs(console);

	/* open no-echo keyboard */
	fd = sysopen(0);
	sprintf(console, "sysopen[%d]: %d", 0, fd);
	sysputs(console);


	/* 1st read of 10 characters from keyboard into user buffer */
	if(fd != -1)	
		sysread(fd, buffer, byte);

	sprintf(console, "sysread[%d]: %s", 0, buffer);
	sysputs(console);


	/* 2nd read of 10 characters from keyboard into user buffer */
	if(fd != -1)	
		sysread(fd, buffer, byte);

	sprintf(console, "sysread[%d]: %s", 0, buffer);
	sysputs(console);


	/* 3rd read of 10 characters from keyboard into user buffer */
	if(fd != -1)	
		sysread(fd, buffer, byte);

	sprintf(console, "sysread[%d]: %s", 0, buffer);
	sysputs(console);

	/* continue reading until eof */
	if(fd != -1)	
		sysread(fd, buffer, byte);
	sprintf(console, "sysread[%d]: %s", 0, buffer);
	sysputs(console);

	/* close no-echo keyboard */
	if(fd != -1)
	{
		fd = sysclose(fd);
		sprintf(console, "sysclose[%d]: %d", 0, fd);
		sysputs(console);
	}

	/* open echo keyboard again */
	fd = sysopen(1);
	sprintf(console, "sysopen[%d]: %d", 1, fd);
	sysputs(console);

	/* install signal 18 */
	syssighandler(18, handler1, &old_handler);

	/* send pid to child proc1 */
	child_pid[0] = syscreate(&proc1, PROC_STACK);
	n=0;
	sprintf(buffer, "%d", n);
	syssend(child_pid[0], buffer, strlen(buffer));

	/* read from echo keyboard, but no user key press is required */
	if(fd != -1)	
		rc = sysread(fd, buffer, byte);

	if(rc == -128)
	{
		sprintf(console, "error sysread: %d", rc);
		sysputs(console);
	}		

	/* send pid to child proc2 */
	child_pid[1] = syscreate(&proc2, PROC_STACK);
	n=0;
	sprintf(buffer, "%d", n);
	syssend(child_pid[1], buffer, strlen(buffer));

	/* install signal 18 */
	syssighandler(18, handler2, &old_handler);

	/* read from echo keyboard, but no user key press is required */
	if(fd != -1)	
		rc = sysread(fd, buffer, byte);

	if(rc == -128)
	{
		sprintf(console, "error sysread: %d", rc);
		sysputs(console);
	}		

	/* install old_handler into signal 20 */
	syssighandler(20, old_handler, &old_handler);

	/* send pid to child proc3 */
	child_pid[2] = syscreate(&proc3, PROC_STACK);
	n=0;
	sprintf(buffer, "%d", n);
	syssend(child_pid[2], buffer, strlen(buffer));


	/* read from echo keyboard, but no user key press is required */
	if(fd != -1)	
		rc = sysread(fd, buffer, byte);

	if(rc == -128)
	{
		sprintf(console, "error sysread: %d", rc);
		sysputs(console);
	}		

	/* continue reading until eof */
	if(fd != -1)	
		sysread(fd, buffer, byte);
	sprintf(console, "sysread[%d]: %s", 1, buffer);
	sysputs(console);

	/* last sysread */
	if(fd != -1)	
		sysread(fd, buffer, byte);
	sprintf(console, "sysread[%d]: %s", 1, buffer);
	sysputs(console);

	sprintf(console, "root complete");
	sysputs(console);
}

/*
* proc1
*
* @desc:	executes the child proc1 process
*/
void proc1 ()
{
	unsigned int buffer[10];
	unsigned int byte=10;
	unsigned int dst=0;
	unsigned int *ptr = &dst;

	/* get root pid */
	byte = sysrecv(ptr, buffer, byte);

	syssleep(1000);

	/* send signal 20 to root process */
	syskill(dst, 20);

	/* send signal 18 to root process */
	syskill(dst, 18);
}

/*
* proc2
*
* @desc:	executes the child proc1 process
*/
void proc2 ()
{
	unsigned int buffer[10];
	unsigned int byte=10;
	unsigned int dst=0;
	unsigned int *ptr = &dst;

	/* get root pid */
	byte = sysrecv(ptr, buffer, byte);

	syssleep(5000);

	/* send signal 20 to root process */
	syskill(dst, 18);
}


/*
* proc2
*
* @desc:	executes the child proc1 process
*/
void proc3 ()
{
	unsigned int buffer[10];
	unsigned int byte=10;
	unsigned int dst=0;
	unsigned int *ptr = &dst;

	/* get root pid */
	byte = sysrecv(ptr, buffer, byte);

	syssleep(5000);

	/* send signal 20 to root process */
	syskill(dst, 20);
}


