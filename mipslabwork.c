/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include <stdlib.h> /* Declaration of stand. library for C --> header file*/
#include "mipslab.h"  /* Declatations for these labs */

/* Define size of display/mips display*/

#define HEIGHT 32
#define WIDTH 128
#define size HEIGHT*WIDTH
#define PIXEL 10
#define PAGES 2

int snakeY = 10; 
int snakeX = 10; 


int mytime = 0x5957;

char textstring[] = "text, more text, and even more text!";

/* Interrupt Service Routine */
void user_isr( void )
{
  return;
}

/* Lab-specific initialization goes here */
void labinit( void )
{
  volatile int* trise = (volatile int*) 0xbf886100;
  *trise &= ~0xff;
  // volatile int* trisf = (volatile int*) 0xbf886140;
  // *trisf |= 0x2;

  volatile int* trisd = (volatile int*) 0xbf8860C0;
  *trisd |= 0xfe0;

  volatile int* porte = (volatile int*) 0xbf886110;
  // volatile int* portf = (volatile int*) 0xbf886150;
  *porte = 0;
  TMR2 = 0;
  PR2 = (80000000/256) / 10;
  T2CON = 0x8070;
  return;
}

/* Version 2
void labinit( void )
{
    volatile int trisE = (volatile int*) 0xbf886100;
    *trisE &= ~(0xff);
    portE &= ~(0xff);

    TRISD = 0x0fe0; //0000 1111 1110 0000 -> s�tter bits fr�n 5 till 11 som input


    T2CON = 0x70;
    PR2 = (80000000/256)/10;  //0.1/(1/80000000)/256;
    TMR2 = 0;
    T2CONSET = 0x8000;

    IEC(0) = 0x100;
    IPC(2) = 0x4;
    enable_interrupt();

  return;
}
*/

int get_snake_head_position() {
  // Beräkna vilken "rad" i den virtuella displayen ormens huvud befinner sig på
  int row_in_display = snakeY / PIXEL;

  // Beräkna och returnera den linjära positionen av ormens huvud i char-arrayen
  int head_position = snakeX + row_in_display * 32;
  return head_position;
}





