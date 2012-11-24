/* kbd.c
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
* @desc:	
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
* kbd_init
*
* @desc:	
*/
pcb_t* kbd_peek()
{
	return kbd_q;
}

/*
* kbd_dequeue
*
* @desc:	
*/
pcb_t* kbd_dequeue()
{
	pcb_t* p = NULL;

	if(kbd_q)
	{
		p = kbd_q;
		kbd_q = kbd_q->next;
	}

	return p;
}

/*
* kbd_dequeue_at
*
* @desc:	
*/
void kbd_dequeue_at(pcb_t *p)
{
	pcb_t *tmp = kbd_q;

        if(!kbd_q) return;
        if(kbd_q->pid == p->pid)
                kbd_q = kbd_q->next;

        while(tmp && tmp->next) 
        {                       
                if(tmp->next->pid == p->pid)
                        tmp->next = tmp->next->next;

                tmp = tmp->next;
        }
}

/*
* puts_kbd_q
*
* @desc:	
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
* @desc:	
*/
void kbd_init()
{
	/* init non-echo kbd */
	dev_table[KBD_NECHO].dvowner  	= NULL;
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
	dev_table[KBD_ECHO].dvowner  	= NULL;
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
* @desc:	
*
* @param:	
*		
*		
*
* @output:	
*/
int kbd_open(devsw_t* d)
{
	/* enable keyboard hardware device */
	enable_irq(1,0);
	return 0;
}

/*
* kbd_close
*
* @desc:	
*
* @param:	
*		
*		
*
* @output:	
*/
int kbd_close(devsw_t* d)
{
	/* disable keyboard hardware device */
	enable_irq(1,1);
	return 0;
}

/*
* kbd_write
*
* @desc:	
*
* @param:	
*		
*		
*
* @output:	
*
* @note:
*/
int kbd_write(devsw_t* d, void* buf, int buflen)
{
	return -1;
}

/*
* kbd_read
*
* @desc:	
*
* @param:	
*		
*		
*
* @output:	
*/
int kbd_read(pcb_t *p, devsw_t* d, void* buf, int buflen)
{
	int *mem = NULL;
	kbdi_t *k = NULL;

	mem = kmalloc(sizeof(kbdi_t));
        k = (kbdi_t *) ((int)mem); 

	k->d = d;
	k->buf = buf;
	k->buflen = buflen;
	k->bufi = 0;

	p->ptr = k;
	
	kbd_enqueue(p);
	return 0;
}

/*
* kbd_ioctl
*
* @desc:	
*
* @param:	
*		
*		
*
* @output:	
*/
int kbd_ioctl(devsw_t* d, unsigned long cmd)
{


	return 0;
}

/*
* kbd_iint
*
* @desc:	
*
* @output:	
*/
int kbd_iint()
{
	unsigned char key;
	unsigned char *buf;
	unsigned char tmp[1];			/* temporary holder of character to concatenated onto the end of the user buffer */
	int i;
	kbdi_t* k = NULL;
	pcb_t* p = NULL;

	if(DATA_READY & inb(CTRL_PORT))
	{		
		key = kbtoa(inb(READY_PORT));

		/* copy typed characters to internal buffer */
		if(key != 0 && key != 4)
		{
			kbd_buf[kbd_buf_i] = key;
			kbd_buf_i++;
		}		

		/* return to user process if ctrl-d is pressed */
		if(key == 4)		
		{
			p = kbd_dequeue();
			p->state = READY_STATE;
			ready(p);
		}

		/* copy character from internal buffer to user buffer */
		if(kbd_q && key != 0)
		{
			k = (kbdi_t*) kbd_q->ptr;

			switch(k->d->dvnum)
			{			
				/* keyboard no echo device, typped characters will only be copied to user buffer */				
				case KBD_NECHO:
					buf = (unsigned char *) k->buf;
				
					if(kbd_buf_i > k->bufi)
					{
						strcpy(buf, kbd_buf);
						k->bufi = kbd_buf_i;
					}		
					else
					{
						strncpy(tmp, kbd_buf+(kbd_buf_i-1), 1); 
						strncat(buf, tmp, 1);
						k->bufi++;
					}	

					/* return to user process if enter is pressed */
					if(key == 10)
					{
						p = kbd_dequeue();
						p->state = READY_STATE;
						ready(p);
					}
			
					break;

				/* keyboard echo device, typped characters will be copied to user buffer and console */				
				case KBD_ECHO:
					buf = (unsigned char *) k->buf;
				
					if(kbd_buf_i > k->bufi)
					{
						strcpy(buf, kbd_buf);
						k->bufi = kbd_buf_i;
					}		
					else
					{
						strncpy(tmp, kbd_buf+(kbd_buf_i-1), 1); 
						strncat(buf, tmp, 1);
						k->bufi++;
					}				

					kprintf("kbd_echo: %s\n", buf);

					/* return to user process if enter is pressed */
					if(key == 10)
					{
						p = kbd_dequeue();
						p->state = READY_STATE;
						ready(p);
					}

					break;
			}

			/* return process if user buffer is full */
			if(k->bufi == k->buflen)
			{	
				p = kbd_dequeue();
				p->state = READY_STATE;
				ready(p);
			}	
		}

		/* reset internal buffer index when size is 4 */
		if(kbd_buf_i == 4)
		{		
			strcpy(kbd_buf, "");
			kbd_buf_i = 0;
		}
	}

	return 0;
}


/*
* kbd_error
*
* @desc:	
*
* @output:	
*/
int kbd_error()
{
	return -1;
}
