/* xeroslib.h
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


int    abs(int arg);
double atof(char *p);
int    atoi(register char *p);
long   atol(register char *p);
/* void   bzero(register char *pch, int len); */

void _doprnt(char *fmt, 	 /* Format string for printf		*/
             int *args,          /* Arguments to printf			*/
             void (*func)(int, unsigned char),      /* Function to put a character		*/
             unsigned int farg); /* Arg to function                     */

int _doscan(register char   *fmt,            /* Format string for the scanf  */
            register int    **argp,          /* Arguments to scanf           */
            int             (*getch)(int, int *),      /* Function to get a character  */
            void             (*ungetch)(int, int *),    /* Function to unget a character*/
            int             arg1,            /* 1st argument to getch/ungetch*/
            int             *arg2);           /* 2nd argument to getch/ungetch*/

char* ecvt(double arg, int ndigits, int *decpt, int *sign);
char* fcvt(double arg, int ndigits, int *decpt, int *sign);
char *fgets(int dev, char *s, int n);
int fprintf(int dev, char *fmt, int args);
int fputs(register char *s,  register int dev);
char *gets(char *s);
char * index(char *sp, char c);
void memset(void *pch, int c, int len);
int printf(char *fmt, int args);
int puts(register char *s);
void qsort(char *a, unsigned n, int es, int (*qcmp)(void *, void *));
void srand(unsigned int x);
int rand(void);
char * rindex(register char *sp, register char c);
int scanf(char * fmt, char args);
int fscanf(int dev, char *fmt, int args);
int sscanf(char * str, char *fmt, int args);
int sprintf(char *str, char *fmt, int args);
char * strcat(register char *s1, register char *s2);
int strcmp(register char *s1, register char *s2) ;
char *strcpy(char *s1, char *s2);
int strlen(register char *s);
char * strncat(register char *s1, register char *s2, register int n);
int strncmp(register char *s1, register char *s2, register int n);
char * strncpy(register char *s1, register char *s2, register int n);
void swab(register short *pf, register short *pt, register int n);
void blkcopy(const void *, void *, int);

extern char _ctype_[];

double ldexp(double, int);
double modf(double, double *);

int getc(int);
int  putc(int, int);
