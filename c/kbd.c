/* kbd.c :	device driver for keyboard (assignment 3)	
* 
* name:         Jack Wu
* student id:   17254079
*/

#include <xeroskernel.h>
#include <scanToASCII.h>
#include <kbd.h>


/* Your code goes here */
extern devsw_t dev_table[DEV_SZ];
static unsigned char kbd_buf[5];
static int kbd_buf_i = 0;	
static unsigned char kbd_eof = 4;		/* init eof to enter */
static unsigned int kbd_free = 4;
static unsigned int kbd_echo_flag = FALSE;
static unsigned int kbd_eof_flag = FALSE;
pcb_t *kbd_q = NULL;

typedef struct kbdi kbdi_t;		/* kbd queue interface */
struct kbdi 
{
	devsw_t* d;
	void* buf;
	int buflen;
	int bufi;
};

/*
* kbd_enqueue
*
* @desc:	place proc on device queue
*
* @param:	p	proc to be placed on queue
*
* @note:	this is an upper layer function
*/
void kbd_enqueue(pcb_t *p)
{
	pcb_t* tmp=kbd_q;
	p->next = NULL;

	if(!kbd_q)
		kbd_q = p;

	while(tmp)
	{
		if(!(tmp->next))
			break;
		tmp=tmp->next;
	}

	tmp->next = p;
}

/*
* kbd_dequeue
*
* @desc:	places head queue proc back on ready_q
*
* @note:	this is an upper layer function
*/
void kbd_dequeue()
{
	pcb_t* p = NULL;

	if(kbd_q)
	{
		/* set proc rc as 0 for eof */
		if(kbd_eof_flag)
			p->rc = 0;

		p = kbd_q;
		kbd_q = kbd_q->next;
		p->state = READY_STATE;
		ready(p);
	}
}

/*
* puts_kbd_q
*
* @desc:	outputs all queued proc pid to console
*/
void puts_kbd_q()
{
	pcb_t *tmp = kbd_q;

	kprintf("kbd_q: ");
        while(tmp) 
        {                  
		kprintf("%d ", tmp->pid);     
                tmp = tmp->next;
        }
}


/*
* kbd_init
*
* @desc:	initialize echo and non-echo keyboard entries in the dev_table
*
* @note:	for unsupported calls, kbd_error is supplied
*		for dvwrite, a kbd_write function is supplied, however this call also always returns -1	
*/
void kbd_init()
{
	/* init non-echo kbd */
	dev_table[KBD_NECHO].dvowner  	= 0;
	dev_table[KBD_NECHO].dvnum    	= KBD_NECHO;
	dev_table[KBD_NECHO].dvinit    	= kbd_error;
	dev_table[KBD_NECHO].dvopen   	= kbd_open;
	dev_table[KBD_NECHO].dvclose  	= kbd_close;
	dev_table[KBD_NECHO].dvread   	= kbd_read;
	dev_table[KBD_NECHO].dvwrite  	= kbd_write;
	dev_table[KBD_NECHO].dvseek  	= kbd_error;
	dev_table[KBD_NECHO].dvgetc  	= kbd_error;
	dev_table[KBD_NECHO].dvputc  	= kbd_error;
	dev_table[KBD_NECHO].dvcntl   	= kbd_ioctl;
	dev_table[KBD_NECHO].dvcsr   	= NULL;
	dev_table[KBD_NECHO].dvivec   	= NULL;
	dev_table[KBD_NECHO].dvovec   	= NULL;
	dev_table[KBD_NECHO].dviint   	= kbd_error;
	dev_table[KBD_NECHO].dvoint   	= kbd_error;
	dev_table[KBD_NECHO].dvioblk   	= NULL;	

	/* init echo kbd */
	dev_table[KBD_ECHO].dvowner  	= 0;
	dev_table[KBD_ECHO].dvnum    	= KBD_ECHO;
	dev_table[KBD_ECHO].dvinit    	= kbd_error;
	dev_table[KBD_ECHO].dvopen   	= kbd_open;
	dev_table[KBD_ECHO].dvclose  	= kbd_close;
	dev_table[KBD_ECHO].dvread   	= kbd_read;
	dev_table[KBD_ECHO].dvwrite  	= kbd_write;
	dev_table[KBD_ECHO].dvseek  	= kbd_error;
	dev_table[KBD_ECHO].dvgetc  	= kbd_error;
	dev_table[KBD_ECHO].dvputc  	= kbd_error;
	dev_table[KBD_ECHO].dvcntl   	= kbd_ioctl;
	dev_table[KBD_ECHO].dvcsr   	= NULL;
	dev_table[KBD_ECHO].dvivec   	= NULL;
	dev_table[KBD_ECHO].dvovec   	= NULL;
	dev_table[KBD_ECHO].dviint   	= kbd_error;
	dev_table[KBD_ECHO].dvoint   	= kbd_error;
	dev_table[KBD_ECHO].dvioblk   	= NULL;
}

/*
* kbd_open
*
* @desc:	enables the keyboard hardware interrupt
*		
* @output:	rc		returns 0 on successful device open
*/
int kbd_open(devsw_t* d)
{
	int i;

	/* enable keyboard hardware device */
	enable_irq(1,0);

	/* enable echo */
	for(i=0 ; i<DEV_SZ ; i++)
	{
		if(dev_table[i].dvowner && i == KBD_ECHO)
			kbd_echo_flag = TRUE;
	}

	return DEV_SUCCESS;
}

/*
* kbd_close
*
* @desc:	disables the keyboard hardware interrupt
*		
* @output:	rc		returns 0 on successful device close
*/
int kbd_close(devsw_t* d)
{
	kbd_echo_flag = FALSE;
	kbd_eof_flag = FALSE;

	/* disable keyboard hardware device */
	enable_irq(1,1);
	return DEV_SUCCESS;
}

/*
* kbd_write
*
* @desc:	unsupported keyboard feature for writing to device buffer
*
* @output:	rc		always returns -1
*/
int kbd_write(devsw_t* d, void* buf, int buflen)
{
	return DEV_ERR;
}

/*
* kbd_read
*
* @desc:	places user buffer on kbd_q
*
* @param:	p		proc to be placed on kbd_q
*		d
*		buf		user buffer
*		buflen		user buffer length
*
* @output:	rc		returns 0 on successful enqueuing
*
* @note:	this is an upper layer function
*/
int kbd_read(pcb_t *p, devsw_t* d, void* buf, int buflen)
{
	int *mem = NULL;
	kbdi_t *k = NULL;


	/* eof flag has been toggled, return proc immediately */
	if(kbd_eof_flag)
	{
		p->rc = 0;
		p->state = READY_STATE;
		ready(p);
	}	

	/* build kbd_q */
	mem = kmalloc(sizeof(kbdi_t));
        k = (kbdi_t *) ((int)mem); 

	strncpy((unsigned char*)buf, "", buflen);
	k->d = d;
	k->buf = buf;
	k->buflen = buflen;
	k->bufi = 0;

	p->ptr = k;	
	kbd_enqueue(p);

	return DEV_SUCCESS;
}

/*
* kbd_ioctl
*
* @desc:	updates the eof character for keyboard input
*
* @param:	eof		new eof character
*		
* @output:	rc		returns 0 on successful update
*
* @note:	this is an upper layer function
*/
int kbd_ioctl(int eof)
{
	kbd_eof = eof;
	return DEV_SUCCESS;
}

/*
* kbd_notify
*
* @desc:	copies device internal buffer data into user buffer
*
* @note:	this is an upper layer function
*/
void kbd_notify()
{
	unsigned char *buf;
	unsigned char tmp[5];			/* temporary holder of character to concatenated onto the end of the user buffer */
	kbdi_t* k = NULL;
	pcb_t* p = NULL;

	int i=kbd_buf_i;
	strncpy(tmp, "", 5);

	/* only transfer characters if a proc has been blocked on device */
	if(kbd_q)
	{
		k = (kbdi_t*) kbd_q->ptr;
		buf = (unsigned char *) k->buf;

		/* copy character from internal buffer to user buffer */
		if( kbd_buf_i > k->buflen) 
		{
			strncpy(tmp, kbd_buf, k->buflen ); 
			kbd_free += k->buflen;
			strncat(buf, tmp, k->buflen);
		}
		else
		{
			strncpy(tmp, kbd_buf, kbd_buf_i); 
			strcat(buf, tmp);
			kbd_free += kbd_buf_i;
		}

		k->bufi += kbd_buf_i;
		kbd_buf_i = 0;

		/* return to user process if enter is pressed */
		/* return process if user buffer is full */
		if(kbd_buf[i-1] == ENTER_KEY || k->bufi >= k->buflen)
		{
			if(k->bufi > k->buflen)
				kbd_q->rc = k->buflen;
			else
				kbd_q->rc = k->bufi;

			kbd_dequeue();
		}
	}
}

/*
* kbd_iint
*
* @desc:	handles keyboard device interrupts
*
* @note:	this is a lower layer function
*/
int kbd_iint()
{
	unsigned char key;

	if(DATA_READY & inb(CTRL_PORT))
	{		
		key = kbtoa(inb(READY_PORT));

		if(key != 0 && key != kbd_eof && kbd_echo_flag)
			kprintf("%c\n", key);


		/* copy typed characters to internal buffer */
		if(key != 0 && key != kbd_eof && kbd_free > 0)
		{
			kbd_buf[kbd_buf_i] = key;
			kbd_buf_i++;
			kbd_free--;
			//kprintf("i: %d	free: %d\n", kbd_buf_i, kbd_free);
		}	

		/* ctrl+d pressed */
		if(key == kbd_eof)
		{
			/* turn on eof flag to return 0 for subsequent reads on the same fd */
			if(!kbd_eof_flag)
				kbd_eof_flag = TRUE;

			kbd_dequeue();
		}


		/* notify the upper kbd layer that a new character has arrived */
		if(key != 0 && !kbd_eof_flag)
			kbd_notify();	
	}

	return DEV_SUCCESS;
}


/*
* kbd_error
*
* @desc:	returns SYSERR for any function not implemented for device
*
* @output:	DEV_ERR
*/
int kbd_error()
{
	kprintf("device function not supported\n");
	return DEV_ERR;
}
