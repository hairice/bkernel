/* kbd.h
* 
* name:         Jack Wu
* student id:   17254079
*/


/* =============== */
/* keyboard device */


/* keyboard device constants*/
#define READY_PORT		0x60
#define CTRL_PORT		0x64
#define DATA_READY		0x1

#define ENTER_KEY		10


#define DEV_SUCCESS		0
#define DEV_ERR			-1

/* keyboard device */
extern void kbd_init();
extern int kbd_open(devsw_t* d);
extern int kbd_close(devsw_t* d);
extern int kbd_write(devsw_t* d, void* buf, int buflen);
extern int kbd_read(pcb_t *p, devsw_t* d, void* buf, int buflen);
extern int kbd_ioctl(int eof);
extern int kbd_iint();
extern int kbd_error();
extern void kbd_notify();


extern void kbd_enqueue(pcb_t *p);
extern void kbd_dequeue();
extern void puts_kbd_q();
