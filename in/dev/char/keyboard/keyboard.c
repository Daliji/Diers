#include"keyboard.h"
#include<int/int.h>
#include<lib/io.h>
#include<dev/char/tty.h>

static int keyboardcount=0;
extern e;

void keyboard()
{
  char scan_code=inb(0x60);
  do_char(scan_code);//放入缓冲队列
  scan_code=inb(0x61);
  //延时
  scan_code=scan_code;
  scan_code=scan_code;
  scan_code|=0x80;
  outb(scan_code,0x61);//禁止键盘中断
  scan_code&=0x7f;
  scan_code=scan_code;
  scan_code=scan_code;
  outb(scan_code,0x61);//允许键盘中断
  send_eoi();//发送eoi中断结束标志
  //  deal_buf();//输入字符处理(secondary)
}

void keyboard_init()
{
  e=0;
  set_intr_gate(0x21,&keyboard_interrupt);//设置中断
  outb(inb_p(0x21)&0xfd,0x21);//启用键盘中断
}
