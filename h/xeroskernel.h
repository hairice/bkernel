/* xeroskernel.h
 *
 * This file defines the macros used in bkernel. 
 *
 * This file is part of bkernel.
 *
 * bkernel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * bkernel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar. If not, see <http://www.gnu.org/licenses/>.
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
#define SYSOK          	 0              /* system call success                  */
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


/* signal return constants */
#define	SIG_SUCCESS			0
#define ERR_SIGKILL_SIG_NO		-3	/* invalid sig number in disp() 							*/
#define ERR_SIGKILL_PROC_NO		-18	/* invalid proc number in disp() 							*/

#define ERR_SIGNAL_PROC_NO		-1	/* invalid proc number in signal() 							*/
#define ERR_SIGNAL_SIG_NO		-2	/* invalid sig number in signal () 							*/
#define ERR_SIGNAL_HANDLER_SIG_NO	-1	/* invalid sig number in siginstall() 							*/
#define ERR_SIGNAL_HANDLER		-2	/* invalid handler address 								*/
#define ERR_SIGNAL_UNBLOCK_SYSCALL	-128	/* system interrupted by signal, this return value does not allow to syssleep () 	*/


/* user process state constants */
#define RUNNING_STATE           0
#define READY_STATE             1
#define SLEEP_STATE             2
#define BLOCK_ON_SEND_STATE     3
#define BLOCK_ON_RECV_STATE     4
#define BLOCK_ON_SIG_STATE     	5
#define BLOCK_ON_DEV_STATE     	6
#define STOP_STATE              7


/* user process constants */
#define INVALID_PID     1               /* errorneous return code for create()          */
#define MIN_PID         1               
#define MAX_PID         65536
#define PROC_SZ        	32              
#define SIG_SZ		32
#define FD_SZ		4
#define DEV_SZ		2

#define RECEIVE_ANY_PID 0               /* ipc_recv call for receiving from any proc    */
#define IDLE_PROC_PID   65536           /* this pid is also used as the pid bound       */
#define PROC_STACK      1024*4          /* set process stack to 4096                    */
#define MIN_STACK       1024    


/* hardware timer constant */
#define CLOCK_DIVISOR   100     


/* sleep constants */
#define BLOCKED_SLEEP	0


/* signal constants */
#define BIT_OFF 	0x0
#define BIT_ON		0x1
#define SIG_ON		0xFFFFFFFF
#define SIG_OFF		0x00000000


/* device constants */
#define KBD_NECHO	0
#define KBD_ECHO	1
#define SET_EOF		100


/* ================================ */
/* interrupt descriptor table entry */
#define TIMER_INT       1
#define KBD_INT       	2
#define KERNEL_INT      64


/* ====================== */
/* system call request id */
#define STOP            100
#define YIELD           101
#define CREATE          102
#define GETPID          103
#define PUTS            104
#define SLEEP           105
#define SEND            106
#define RECV            107

#define SIG_HANDLER	1000
#define SIG_RETURN	1001
#define SIG_KILL	1002
#define SIG_WAIT	1003

#define DEV_OPEN	2000
#define DEV_CLOSE	2001
#define DEV_WRITE	2002
#define DEV_READ	2003
#define DEV_IOCTL	2004


/* ========= */
/* test mode */


/* ============== */
/* ipc send tests */
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


/* ================= */
/* ipc receive tests */
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


/* ================ */
/* time share tests */
#ifndef TIME_TEST
/* uncomment to enable time_share tests, once this is uncommented timetest_root() will be created and 1 time_share test will be enabled */
//#define TIME_TEST 
#endif


/* ===================== */
/* signal handling tests */
#ifndef SIG_TEST
/* uncomment to enable signal tests, once this is uncommented sigtest_root() will be created */
//#define SIG_TEST


#ifndef SIG_INSTALL_TEST
/* uncomment to enable sig_install_test */
//#define SIG_INSTALL_TEST 
#endif

#ifndef SIG_KILL_TEST
/* uncomment to enable sig_kill_test */
//#define SIG_KILL_TEST
#endif

#ifndef SIG_WAIT_TEST
/* uncomment to enable sig_wait_test */
//#define SIG_WAIT_TEST
#endif

#ifndef SIG_PRIO_TEST
/* uncomment to enable sig_prio_test */
//#define SIG_PRIO_TEST
#endif

#ifndef SIG_SLEEP_TEST
/* uncomment to enable sig_sleep_test */
//#define SIG_SLEEP_TEST
#endif


#endif


/* =================== */
/* device driver tests */
#ifndef DEV_TEST
/* uncomment to enable device driver tests, once this is uncommented devtest_root() will be created */
//#define DEV_TEST


#ifndef DEV_OPEN_TEST
/* uncomment to enable dev_open_test */
//#define DEV_OPEN_TEST
#endif

#ifndef DEV_WRITE_TEST
/* uncomment to enable dev_write_test */
//#define DEV_WRITE_TEST
#endif

#ifndef DEV_IOCTL_TEST
/* uncomment to enable dev_ioctl_test */
//#define DEV_IOCTL_TEST
#endif

#ifndef DEV_READ_TEST
/* uncomment to enable dev_read_test */
//#define DEV_READ_TEST
#endif

#ifndef DEV_EOF_TEST
/* uncomment to enable dev_eof_test */
//#define DEV_EOF_TEST
#endif


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
	unsigned int pid;
        unsigned int *pid_ptr;          /* desired pid to send/receive in ipc communication             */
        void *buffer;                   /* holds the data that will be transmitted to/from between proc */
        int buffer_len;                 /* the length of data transfer acceptance at one end of ipc     */
};

typedef struct fd fd_t;
struct fd
{
	int dvmajor;
};

typedef struct pcb pcb_t;
struct pcb 
{
        unsigned int pid;               /* process pid                                                                  */
        unsigned int state;             /* process state currently in the system                                        */
        unsigned int esp;               /* process stack pointer                                                        */
        unsigned int *mem;              /* process memory 'dataStart' pointer                                           */
        unsigned int args;              /* retains all arguments passed from a syscall()                                */
        int rc;                		/* return code from syscall()                                                   */

	unsigned int sig_table[SIG_SZ];	/* user process signal table 							*/
	unsigned int sig_pend_mask;	/* all signals signals targetted to the proc 					*/
	unsigned int sig_install_mask;	/* signals with an installed handler 						*/
	unsigned int sig_ignore_mask;	/* ignored signals (toggled as 0) 						*/

        unsigned int delta_slice;       /* process time slices to sleep for,
                                        *  this value is stored as a key in the delta list for sleep queue              */

        void *ptr;                      /* generic pointer, as of a2, this pointer is used to reference the ipc data    */

	fd_t fd_table[FD_SZ];		/* table of file descriptors */

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

typedef struct devsw devsw_t;
struct devsw 
{
	unsigned int dvowner;		/* proc which is currently using this device 	*/
	unsigned int dvnum;		/* dev major number 				*/
	char *devname;
	int (*dvinit)();		
	int (*dvopen)();
	int (*dvclose)();
	int (*dvread)();		/* dev read interface 				*/
	int (*dvwrite)();
	int (*dvseek)();
	int (*dvgetc)();
	int (*dvputc)();
	int (*dvcntl)();		/* manipulate keyboard eof			*/
	void *dvcsr;
	void *dvivec;
	void *dvovec;
	int (*dviint)();		
	int (*dvoint)();
	void *dvioblk;
	int dvminor;
};

pcb_t proc_table[PROC_SZ];             	/* list of process control blocks       */
pcb_t *stop_q;           		/* stop queue for pcb                   */
devsw_t dev_table[DEV_SZ];		/* list of devices 			*/


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

extern int syssighandler(int sig_no, void (*new_handler)(void *), void (** old_handler)(void *));
extern void sigreturn(void *old_sp, int old_rc, unsigned int old_im);
extern int syskill(int pid, int sig_no);
extern int syssigwait(void);

extern int sysopen(int device_no);
extern int sysclose(int fd);
extern int syswrite(int fd, void *buff, int bufflen);
extern int sysread(int fd, void *buff, int bufflen);
extern int sysioctl(int fd, unsigned long command, ...);


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
extern void wake_early(pcb_t *p);
extern unsigned int sleeper (void);                     /* count number of proc pcb in the sleep_q                      */
extern unsigned int sleep_to_slice (unsigned int ms);   /* convert ms to number of slices, ms / (CLOCK_DIVISOR/10)      */
extern void puts_sleep_q(void);


/* hardware timer */
extern unsigned int tick(void);


/* ipc */
extern void send(pcb_t* p, unsigned int pid, void *buffer, int buffer_len);
extern void recv(pcb_t* p, unsigned int *pid, void *buffer, int buffer_len);


/* signal processing */
extern int siginstall(pcb_t *p, int sig_no, void (*new_handler)(void *), void (** old_handler)(void *));	/* install signal for user proc 		*/
extern int signal(int pid, int sig_no);										/* set target signal bit for proc		*/
extern int sighigh(pcb_t *p);											/* pick highest signal and deliver to proc 	*/
extern void sigtramp(void (*handler)(void *), void *cntx, void *osp, int rc, unsigned int oim);					
extern int sigdeliver(int pid, int sig_no);									/* place signal stack in user space 		*/
extern void sigcease(pcb_t *p, unsigned int oim);								/* restore previous ignore mask 		*/
extern void puts_sig_table(pcb_t *p);
extern void puts_sig_mask(void);


/* device driver */
extern int di_open(pcb_t *p, int device_no);
extern int di_close(pcb_t *p, int fd);
extern int di_write(pcb_t *p, int fd, void *buf, int buflen);
extern int di_read(pcb_t *p, int fd, void *buf, int buflen);
extern int di_ioctl(pcb_t *p, int fd, unsigned long command, ...);

/* test processes */
extern void sndtest_root(void);
extern void rcvtest_root(void);
extern void timetest_root(void);
extern void sigtest_root(void);
extern void devtest_root(void);
