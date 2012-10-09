/* xeroskernel.h - disable, enable, halt, restore, isodd, min, max */

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
#define PROC_STACK	1024*4

#define STOP        	0
#define YIELD      	1
#define CREATE     	2



#ifndef MEM_DEBUG
// Uncomment line to enable memory debug prints
//#define MEM_DEBUG
#endif

#ifndef TEST_MODE
// Uncomment line to execute 2 memory managment test cases and 2 process management test cases
//#define TEST_MODE

// The 4 test cases have been divided into two separate macros, as this is a workaround for the limited amount of
// console viewing space when executing in bochs
#ifndef MEM_TEST
// Uncomment to enable memory managment test cases print out
//#define MEM_TEST
#endif

#ifndef PROC_TEST
// Uncomment to enable process managment test cases print out
//#define PROC_TEST
#endif

#endif

typedef struct memHeader memHeader_t;
struct memHeader 
{ 
	unsigned long size; 		// Size of the memory block after the memory header
	memHeader_t *prev; 		// Link to previous memory block that is lower in address
	memHeader_t *next;  		// Link to next memory block that is higher in address
	char *sanityCheck; 		// Corrupt check for memory block
	unsigned char dataStart[0]; 	// Start of the data portion of a memory block
}; 

typedef struct pcb pcb_t;
struct pcb 
{
	int pid;			// Process pid
	int esp;			// Process stack pointer
	int *mem;			// Process memory 'dataStart' pointer
	long args;			// Retains all arguments passed from a system call
	pcb_t *next;			// Link to the next pcb block, two queues exist in the os, ready and stop
};

pcb_t proc_table[MAX_PROC];
pcb_t *stop_q;

typedef struct context_frame context_frame_t;
struct context_frame 
{
	unsigned int   edi;		//
	unsigned int   esi;		//
	unsigned int   ebp;		// Process frame pointer
	unsigned int   esp;		// Process stack pointer
	unsigned int   ebx;		//
	unsigned int   edx;		// Process data register
	unsigned int   ecx;		//
	unsigned int   eax;		// Process return value register
	unsigned int   iret_eip;	// Process instruction pointer
	unsigned int   iret_cs;		// Process code segment start
	unsigned int   eflags;		// Process error flags
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

// Memory Managment Unit
extern void kmeminit(void);
extern void *kmalloc(int size);
extern void kfree(void *ptr);
extern void kmemprint(void);
extern int kmemhdsize(void);
extern int kmemtotalsize(void);

// Process Management Unit
extern void dispatch(void);
extern pcb_t* next(void);
extern void ready(pcb_t *p);
extern void stop(pcb_t *p);
extern int count(void);

extern void contextinit(void);
extern int contextswitch(pcb_t *p);
extern int create(void (*func)(void), int stack); 

// System Calls
extern int syscall(int call, ...);
extern int syscreate(void (*func)(void), int stack);
extern void sysyield(void);
extern void sysstop(void);

// User Processes
extern void root(void);
extern void producer(void);
extern void consumer(void);

// Test Driver
extern void testdriver(void);
extern void testroot(void);
extern void testproc(void);