/* Keyboard Device Driver
 *
 * This file defines the macros for the keyboard device driver
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
