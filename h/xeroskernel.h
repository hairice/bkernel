/* xeroskernel.h - disable, enable, halt, restore, isodd, min, max 
* 
* name:         Jack Wu
* student id:   17254079
*/



/* ==================== */
/* xeros defined macros */
typedef char            Bool;           /* boolean type                         */
#define FALSE           0               /* boolean constants                    */
#define TRUE            1
#define EMPTY           (-1)            /* an illegal gpq                       */
#define NULL            0               /* null pointer for linked lists        */
#define NULLCH          '\0'            /* The null character                   */


/* universal return constants */
#define OK               1              /* system call ok                       */
#define SYSERR          -1              /* system call failed                   */
#define EOF             -2              /* end-of-file (usu. from read)         */
#define TIMEOUT         -3              /* time out  (usu. recvtim)             */
#define INTRMSG         -4              /* keyboard "intr" key pressed          */
                                        /*  (usu. defined as ^B)                */
#define BLOCKERR        -5              /* non-blocking op would block          */


/* ipc return constants */
#define ERR_PID         -1              /* invalid pid of sender/receiver       */
#define ERR_LOOPBACK    -2              /* sysend to the pid itself             */
#define ERR_IPC         -3              /* any other ipc errors                 */


/* interrupt descriptor table entry */
#define TIMER_INT       1
#define KERNEL_INT      64


/* system call request id */
#define STOP            100
#define YIELD           101
#define CREATE          102
#define GETPID          103
#define PUTS            104
#define SLEEP           105
#define SEND            106
#define RECV            107


/* process states */
#define RUNNING_STATE           0
#define READY_STATE             1
#define SLEEP_STATE             2
#define BLOCK_ON_SEND_STATE     3
#define BLOCK_ON_RECV_STATE     4
#define STOP_STATE              5


/* hardware timer constant */
#define CLOCK_DIVISOR   100     


/* user process constants */
#define INVALID_PID     1               /* errorneous return code for create()          */
#define MIN_PID         1               
#define MAX_PID         65536
#define PROC_SZ        	32              
#define SIG_SZ		32

#define RECEIVE_ANY_PID 0               /* ipc_recv call for receiving from any proc    */
#define IDLE_PROC_PID   65536           /* this pid is also used as the pid bound       */
#define PROC_STACK      1024*4          /* set process stack to 4096                    */
#define MIN_STACK       1024            


/* sleep constants */
#define BLOCKED_SLEEP	0


/* ========= */
/* test mode */
#ifndef SEND_TEST
/* uncomment to enable ipc_send tests, once this is uncommented sndtest_root() will be created */
//#define SEND_TEST 

#ifndef SEND_POSITIVE_TEST
/* uncomment to enable 2 success ipc_send tests */
//#define SEND_POSITIVE_TEST 
#endif

#ifndef SEND_NEGATIVE_TEST
/* uncomment to enable 2 failure ipc_send tests */
//#define SEND_NEGATIVE_TEST 
#endif

#endif


#ifndef RECV_TEST
/* uncomment to enable ipc_recv tests, once this is uncommented rcvtest_root() will be created */
//#define RECV_TEST 

#ifndef RECV_POSITIVE_TEST
/* uncomment to enable 2 success ipc_recv tests */
//#define RECV_POSITIVE_TEST 
#endif

#ifndef RECV_NEGATIVE_TEST
/* uncomment to enable 2 failure ipc_recv tests */
//#define RECV_NEGATIVE_TEST 
#endif

#endif


#ifndef TIME_TEST
/* uncomment to enable time_share tests, once this is uncommented timetest_root() will be created and 1 time_share test will be enabled */
//#define TIME_TEST 
#endif


/* ====================== */
/* system data structures */
typedef struct memHeader memHeader_t;
struct memHeader 
{ 
        unsigned long size;             /* size of the memory block after the memory header             */
        memHeader_t *prev;              /* link to previous memory block that is lower in address       */
        memHeader_t *next;              /* link to next memory block that is higher in address          */
        char *sanityCheck;              /* corrupt check for memory block                               */
        unsigned char dataStart[0];     /* start of the data portion of a memory block                  */
}; 


typedef struct ipc ipc_t;               
struct ipc
{
        unsigned int *pid_ptr;          /* desired pid to send/receive in ipc communication             */
        void *buffer;                   /* holds the data that will be transmitted to/from between proc */
        int buffer_len;                 /* the length of data transfer acceptance at one end of ipc     */
};

typedef struct pcb pcb_t;
struct pcb 
{
        unsigned int pid;               /* process pid                                                                  */
        unsigned int state;             /* process state currently in the system                                        */
        unsigned int esp;               /* process stack pointer                                                        */
        unsigned int *mem;              /* process memory 'dataStart' pointer                                           */
        unsigned int args;              /* retains all arguments passed from a syscall()                                */

        unsigned int delta_slice;       /* process time slices to sleep for,
                                        *  this value is stored as a key in the delta list for sleep queue              */

	unsigned int sig_table[SIG_SZ];	/* user process signal table 							*/

        void *ptr;                      /* generic pointer, as of a2, this pointer is used to reference the ipc data    */
        unsigned int rc;                /* return code from syscall()                                                   */

        pcb_t *blocked_senders;         /* queue of blocked senders for a proc */
        pcb_t *blocked_receivers;       /* queue of blocked receivers for a proc */     
        pcb_t *next;                    /* link to the next pcb block, two queues exist in the os, ready and stop       */
};

typedef struct context_frame context_frame_t;
struct context_frame 
{
        unsigned int   edi;             
        unsigned int   esi;             
        unsigned int   ebp;             /* process frame pointer                                */
        unsigned int   esp;             /* process stack pointer                                */
        unsigned int   ebx;             
        unsigned int   edx;             /* process data register (keeps args from syscall())    */
        unsigned int   ecx;             /* process interrupt code                               */
        unsigned int   eax;             /* process return value register                        */
        unsigned int   iret_eip;        /* process instruction pointer                          */
        unsigned int   iret_cs;         /* process code segment start                           */
        unsigned int   eflags;          /* process error flags                                  */

        unsigned int   iret_func;       /* process implicit function call at the end of the stack, this function is switched into,
                                        *  however it is not returned back from
                                        */
};

pcb_t proc_table[PROC_SZ];             /* list of process control blocks       */
pcb_t *stop_q;                          /* stop queue for pcb                   */


/* ================= */
/* library functions */
void bzero(void *base, int cnt);
void bcopy(const void *src, void *dest, unsigned int n);
int kprintf(char * fmt, ...);
void lidt(void);
void init8259(void);
void disable(void);
void outb(unsigned int, unsigned char);
unsigned char inb(unsigned int);



/* ========================= */
/* cpsc415 defined functions */

/* memory managment unit */
extern void kmeminit(void);
extern void *kmalloc(int size);
extern void kfree(void *ptr);
extern void kmemprint(void);
extern int kmemhdsize(void);            /* get size of head free mem block              */
extern int kmemtotalsize(void);         /* get total size from all free mem blocks      */


/* process management unit */
extern void dispatch(void);
extern pcb_t* next(void);                               /* get read_q head proc pcb                             */
extern void ready(pcb_t *p);                            /* put proc pcb in the ready_q                          */
extern void stop(pcb_t *p);                             /* put proc pcb in the stop_q                           */
extern void block(pcb_t **q, pcb_t *p);                 /* put proc pcb in the block_q                          */
extern pcb_t* unblock(pcb_t **q, unsigned int pid);     /* get proc pcb in the block_q                          */
extern Bool deadlock(pcb_t *q, pcb_t *p);               /* deadlock detection for ipc send/receive              */
extern void release(pcb_t **q);                         /* releases blocked sender/receiver back into ready_q   */
extern pcb_t* get_proc(int pid);                        /* get pcb_t from the proc_table of the provided pid    */
extern int count(void);                                 /* get number of proc pcb in the ready_q                */
void puts_ready_q(void);                                
void puts_blocked_q(void);
void puts_receive_any (void);

extern void contextinit(void);
extern int contextswitch(pcb_t *p);
extern int create(void (*func)(void), int stack); 
extern unsigned int find_pid(void);                     /* return next available pid within MIN_PID and MAX_PID */
extern void set_max_pid(void);
extern void set_min_pid(void);


/* system calls */
extern int syscall(int call, ...);
extern int syscreate(void (*func)(void), int stack);
extern void sysyield(void);
extern void sysstop(void);


/* auxiliary system calls */
extern unsigned int sysmalloc (unsigned int size);
extern int syssend(unsigned int dest_pid, void *buffer, int buffer_len);
extern int sysrecv(unsigned int *from_pid, void *buffer, int buffer_len);
extern unsigned int syssleep(unsigned int milliseconds);
extern unsigned int sysgetpid(void);
extern void sysputs(char *str);


/* user processes */
extern void idleproc (void);
extern void root(void);
extern void proc1(void);
extern void proc2(void);
extern void proc3(void);
extern void proc4(void);


/* sleep device */
extern unsigned int sleep(pcb_t *p);    
extern void wake(void);                                 /* get head proc pcb in the sleep_q                             */
extern unsigned int sleeper (void);                     /* count number of proc pcb in the sleep_q                      */
extern unsigned int sleep_to_slice (unsigned int ms);   /* convert ms to number of slices, ms / (CLOCK_DIVISOR/10)      */
extern void puts_sleep_q(void);


/* hardware timer */
extern unsigned int tick(void);


/* ipc */
extern void send(pcb_t* p, unsigned int pid, void *buffer, int buffer_len);
extern void recv(pcb_t* p, unsigned int *pid, void *buffer, int buffer_len);


/* signal processing */
extern int syssighandler(int signal, void (*newhandler)(void *), void (** oldHandler)(void *));
extern void sigreturn(void *old_sp);
extern int syskill(int pid, int sig_no);
extern int syssigwait(void);
extern void sigtramp(void (*handler)(void *), void *cntx, void *osp);
extern int signal(int pid, int sig_no);


/* test processes */
extern void sndtest_root(void);
extern void rcvtest_root(void);
extern void timetest_root(void);
