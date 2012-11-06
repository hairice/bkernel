/* snd_test.c : ipc_send test cases
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>
#define MAX_CNT	5

/* test driver */
extern void timetest_proc1(void);
extern void timetest_proc2(void);
extern void timetest_proc3(void);

static unsigned int cnt=0;
static Bool result[3];

/*
* test_root
*
* @desc:	executes the testroot process
* @note:	this process executes the process management test cases
*/	
void timetest_root(void)
{
	int child_pid[3],pid,i;
	pid = sysgetpid();

#ifdef TIME_TEST
	child_pid[0] = syscreate(&timetest_proc1, PROC_STACK);
	child_pid[1] = syscreate(&timetest_proc2, PROC_STACK);
	child_pid[2] = syscreate(&timetest_proc3, PROC_STACK);

	kprintf("-----------------------------------------------------------\n");
	kprintf("proc\t\t\tstate\t\t\t\tcnt\n");
	kprintf("-----------------------------------------------------------\n");	
#endif

	syssleep(7000);

	/*  
	* @test: 	time_share_pos_1
	*
	* @desc:	the child processes are able to be preempted by the kernel and share cpu resource by not exceeding its quantum
	*
	* @outcome:	each process will output 5 console lines
	*
	* @note:	this test case is done without sysyield(), the processes are expected to be switched to another processes involuntarily,
	*		also, only 5 console lines are printed per process, because this is to ensure the entire print stack is still in the viewing 
	*		console at the end of the test
	*/

	/* output test results */
	for(i=0 ; i<3 ; i++)
	{
		if(result[i] == FALSE)
		{
			kprintf("\n\ntest\t\t\tresult\t\tcomment\n");
			kprintf("-----------------------------------------------------------\n");
			kprintf("time_share_pos_1\tfail\t\tp%d did not exec %d times\n", child_pid[i], MAX_CNT);
			break;
		}
		
		if(i == 2)
		{
			kprintf("\n\ntest\t\t\tresult\t\tcomment\n");
			kprintf("-----------------------------------------------------------\n");
			kprintf("time_share_pos_1\tpass\t\tall proc have exec %d times\n", MAX_CNT);
		}
	}
}

/*
* test_proc1
*
* @desc:	executes the testproc process
*/
void timetest_proc1(void)
{
#ifdef TIME_TEST
	unsigned int pid,i=0,cnt=0;
	pid = sysgetpid();
	
	for(;;) 
	{
		if(i < 300000) i++;
		else
		{	
			if(cnt > MAX_CNT)
			{
				result[0] = FALSE;
				break;
			}

			if(cnt == MAX_CNT)
			{
				result[0] = TRUE;
				break;
			}

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
#ifdef TIME_TEST
	unsigned int pid,i=0,cnt=0;
	pid = sysgetpid();
	
	for(;;) 
	{
		if(i < 300000) i++;
		else
		{	
			if(cnt > MAX_CNT)
			{
				result[1] = FALSE;
				break;
			}

			if(cnt == MAX_CNT)
			{
				result[1] = TRUE;
				break;
			}

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
#ifdef TIME_TEST
	unsigned int pid,i=0,cnt=0;
	pid = sysgetpid();	

	for(;;) 
	{
		if(i < 300000) i++;
		else
		{
			if(cnt > MAX_CNT)
			{
				result[2] = FALSE;
				break;
			}

			if(cnt == MAX_CNT)
			{
				result[2] = TRUE;
				break;
			}

			cnt++;
			kprintf("[p%d]\t\t\t[running]\t\t\t[%d]\n", pid, cnt);
			i=0;
		}		
	}
#endif
}
