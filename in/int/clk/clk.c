#include<lib/io.h>
#include<int/int.h>
#include"clk.h"

void clock_int_start();

void clock_init()
{
  outb_p(0x36,0x43);
  outb_p(LATCH&0xff,0x40);
  outb_p(LATCH>>8,0x40);
  set_intr_gate(0x20,&clock_int_start);
  outb(inb_p(0x21)&0xfe,0x21);
}

void playsound(int hz,int time)
{
  int number=CLK_HZ/hz*2;
  int count=time*hz/1000;
  outb(0xb6,0x43);
  outb(number&0xff,0x42);
  outb(number>>8,0x42);
  outb(inb(0x61)|0x03,0x61);
  __asm__("movl %0,%%ecx\n\t"
	  "L1:\tinb $0x62,%%al\n\t"
	  "testb $0x20,%%al\n\t"
	  "jz L1\n\t"
	  "L2:\tinb $0x62,%%al\n\t"
	  //测试
	  "movl $0xb8000,%%esi\n\t"
	  "addb $16,%%al\n\t"
	  "movb %%al,(%%esi)\n\t"
	  "subb $16,%%al\n\t"
	  //测试结束
	  "testb $0x20,%%al\n\t"
	  "jnz L2\n\t"
	  "dec %%ecx\n\t"
	  "jnz L1\n\t"
	  ::"g"(count));
  outb(inb(0x61)&0xfe,0x61);
}

void clk_init()
{
  clock_init();
  sount_init();
}
