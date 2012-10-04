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


typedef struct memHeader memHeader_t;
struct memHeader 
{ 
	unsigned long size; 
	memHeader_t *prev; 
	memHeader_t *next; 
	char *sanityCheck; 
	unsigned char dataStart[0]; 
}; 

typedef struct pcb pcb_t;
struct pcb 
{
	int pid;
	int esp;
	int *mem;
	long args;
	pcb_t *next;
};

pcb_t proc_table[MAX_PROC];
pcb_t *stop_q;

typedef struct context_frame context_frame_t;
struct context_frame 
{
	unsigned int   edi;
	unsigned int   esi;
	unsigned int   ebp;
	unsigned int   esp;
	unsigned int   ebx;
	unsigned int   edx;
	unsigned int   ecx;
	unsigned int   eax;
	unsigned int   iret_eip;
	unsigned int   iret_cs;
	unsigned int   eflags;
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

// Process Management Unit
extern void dispatch();
extern pcb_t* next();
extern void ready(pcb_t *p);
extern void stop(pcb_t *p);

extern int contextswitch(pcb_t *p);
extern int create(void (*func)(), int stack); 

// System Calls
extern int syscall(int call, ...);
extern int syscreate();
extern void sysyield();
extern void sysstop();

// User Processes
extern void root();
extern void producer();
extern void consumer();



