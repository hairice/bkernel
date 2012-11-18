/* kbd.h
* 
* name:         Jack Wu
* student id:   17254079
*/



extern int kbd_init(devsw_t* d);
extern int kbd_open(devsw_t* d);
extern int kbd_close(devsw_t* d);
extern int kbd_write(devsw_t* d, void* buf, int buflen);
extern int kbd_read(devsw_t* d, void* buf, int buflen);
extern int kbd_ioctl(devsw_t* d, unsigned long cmd); 
extern int kbd_iint();
