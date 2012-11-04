/* snd_test.c : ipc_send test cases
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>
#define MAX_CNT	30

/* test driver */
extern void timetest_proc1(void);
extern void timetest_proc2(void);
extern void timetest_proc3(void);

static unsigned int cnt=0;

/*
* test_root
*
* @desc:	executes the testroot process
* @note:	this process executes the process management test cases
*/	
void timetest_root(void)
{
	int child_pid[3],pid;
	pid = sysgetpid();

#ifdef TIME_TEST
	child_pid[0] = syscreate(&timetest_proc1, PROC_STACK);
	child_pid[1] = syscreate(&timetest_proc2, PROC_STACK);
	child_pid[2] = syscreate(&timetest_proc3, PROC_STACK);

	kprintf("-----------------------------------------------------------\n");
	kprintf("proc\t\t\tstate\t\t\t\tcnt\n");
	kprintf("-----------------------------------------------------------\n");	
#endif
}

/*
* test_proc1
*
* @desc:	executes the testproc process
*/
void timetest_proc1(void)
{
	unsigned int pid,i=0,cnt=0;
	Bool puts_console = FALSE;
	pid = sysgetpid();
	

#ifdef TIME_TEST
	for(;;) 
	{
		if(i < 600000) i++;
		else
		{	
			cnt++;
			kprintf("[p%d]\t\t\t[running]\t\t\t[%d]\n", pid, cnt);
			i=0;
		}		
	}
#endif
}

/*
* test_proc2
*
* @desc:	executes the testproc process
*/
void timetest_proc2(void)
{
	unsigned int pid,i=0,cnt=0;
	Bool puts_console = FALSE;
	pid = sysgetpid();
	

#ifdef TIME_TEST
	for(;;) 
	{
		if(i < 700000) i++;
		else
		{	
			cnt++;
			kprintf("[p%d]\t\t\t[running]\t\t\t[%d]\n", pid, cnt);
			i=0;
		}		
	}
#endif
}

/*
* test_proc3
*
* @desc:	executes the testproc process
*/
void timetest_proc3(void)
{
	unsigned int pid,i=0,cnt=0;
	Bool puts_console = FALSE;
	pid = sysgetpid();
	

#ifdef TIME_TEST
	for(;;) 
	{
		if(i < 800000) i++;
		else
		{
			cnt++;
			kprintf("[p%d]\t\t\t[running]\t\t\t[%d]\n", pid, cnt);
			i=0;
		}		
	}
#endif
}
