/* xeroskernel.h - disable, enable, halt, restore, isodd, min, max 
* 
* name:		Jack Wu
* student id:	17254079
*/

/* Symbolic constants used throughout Xinu */

typedef	char		Bool;		/* Boolean type			*/
#define	FALSE		0		/* Boolean constants		*/
#define	TRUE		1
#define	EMPTY		(-1)		/* an illegal gpq		*/
#define	NULL		0		/* Null pointer for linked lists*/
#define	NULLCH		'\0'		/* The null character		*/


/* Universal return constants */
#define	OK		 1		/* system call ok		*/
#define	SYSERR		-1		/* system call failed		*/
#define	EOF		-2		/* End-of-file (usu. from read)	*/
#define	TIMEOUT		-3		/* time out  (usu. recvtim)	*/
#define	INTRMSG		-4		/* keyboard "intr" key pressed	*/
					/*  (usu. defined as ^B)	*/
#define	BLOCKERR	-5		/* non-blocking op would block	*/

#define MAX_PROC	32
#define KERNEL_INT	64
#define PROC_STACK	1024*4		/* set process stack to 4096 */

/* syscall() request id */
#define STOP        	0
#define YIELD      	1
#define CREATE     	2
#define GETPID		3
#define PUTS		4
#define SLEEP		5

#ifndef MEM_DEBUG
/* uncomment line to enable memory debug prints */
/* #define MEM_DEBUG */
#endif

#ifndef TEST_MODE
/* uncomment line to enable test driver and disable normal os execution */
/* #define TEST_MODE */

/*
* the 4 test cases have been divided into two separate macros, as this is a workaround 
* for the limited amount of console viewing space when executing in bochs 
*/
#ifndef MEM_TEST
/* uncomment line to enable memory managment test cases */
/* #define MEM_TEST */
#endif

#ifndef PROC_TEST
/* uncomment line to enable process managment test cases */
/* #define PROC_TEST */
#endif


#endif

typedef struct memHeader memHeader_t;
struct memHeader 
{ 
	unsigned long size; 		/* size of the memory block after the memory header */
	memHeader_t *prev; 		/* link to previous memory block that is lower in address */
	memHeader_t *next;  		/* link to next memory block that is higher in address */
	char *sanityCheck; 		/* corrupt check for memory block */
	unsigned char dataStart[0]; 	/* start of the data portion of a memory block */
}; 

typedef struct pcb pcb_t;
struct pcb 
{
	unsigned int pid;		/* process pid */
	unsigned int esp;		/* process stack pointer */
	unsigned int *mem;		/* process memory 'dataStart' pointer */
	unsigned int args;		/* retains all arguments passed from a system call */
	unsigned int rc;
	pcb_t *next;			/* link to the next pcb block, two queues exist in the os, ready and stop */
};

pcb_t proc_table[MAX_PROC];		/* list of process control blocks */
pcb_t *stop_q;				/* stop queue for pcb */

typedef struct context_frame context_frame_t;
struct context_frame 
{
	unsigned int   edi;		
	unsigned int   esi;		
	unsigned int   ebp;		/* process frame pointer */
	unsigned int   esp;		/* process stack pointer */
	unsigned int   ebx;		
	unsigned int   edx;		/* process data register (keeps args from syscall()) */
	unsigned int   ecx;		
	unsigned int   eax;		/* process return value register */
	unsigned int   iret_eip;	/* process instruction pointer */
	unsigned int   iret_cs;		/* process code segment start */
	unsigned int   eflags;		/* process error flags */
	unsigned int   iret_func;	/* process implicit function call at the end of the stack */
};

/* Functions defined by startup code */
void bzero(void *base, int cnt);
void bcopy(const void *src, void *dest, unsigned int n);
int kprintf(char * fmt, ...);
void lidt(void);
void init8259(void);
void disable(void);
void outb(unsigned int, unsigned char);
unsigned char inb(unsigned int);


/* Assignment 1 Functions */

/* memory managment unit */
extern void kmeminit(void);
extern void *kmalloc(int size);
extern void kfree(void *ptr);
extern void kmemprint(void);
extern int kmemhdsize(void);
extern int kmemtotalsize(void);

/* process management unit */
extern void dispatch(void);
extern pcb_t* next(void);
extern void ready(pcb_t *p);
extern void stop(pcb_t *p);
extern int count(void);

extern void contextinit(void);
extern int contextswitch(pcb_t *p);
extern int create(void (*func)(void), int stack); 

/* system calls */
extern int syscall(int call, ...);
extern int syscreate(void (*func)(void), int stack);
extern void sysyield(void);
extern void sysstop(void);

/* auxiliary system calls */
extern unsigned int sysgetpid(void);
extern void sysputs( char *str );

extern unsigned int syssleep( unsigned int milliseconds );

/* user processes */
extern void root(void);
extern void producer(void);
extern void consumer(void);


/* sleep device */
extern void sleep(void);
extern void tick(void);


/* test driver */
extern void testdriver(void);
extern void testroot(void);
extern void testproc(void);
