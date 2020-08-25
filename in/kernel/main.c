#include"main.h"
#include<mm/mm.h>
#include<int/interrupt/interrupt.h>
#include<int/clk/clk.h>
#include<lib/io.h>
#include<mm/pro/process.h>

unsigned long sys_tick;/*系统时间片*/
char start_process_disp;/*时间片到达时允许进程调度标志*/

/*初始化2号进程*/
void init_2(){
  int i;
  int j;
  i=system_call_00(0);
  i=system_call_00(0);
  i=system_call_00(0);
  if(i>0){
    //  system_call_00(4);
    system_call_00(1);
    //    system_call_00(3);
  }else if(i==0){
    system_call_00(2);
    //    system_call_00(3);
  }
  while(1);
}

void e_main()
{
  int i;
  sys_tick=0;/*初始时间片（系统滴答数）为0*/
  start_process_disp=0;/*不允许时间片进程调度*/
  init_memory();/*内存初始化，具体请见mm/mm.c*/
  init_int();/*中断初始化，具体请见int/interrupt/interrupt.c*/
  tty_init();/*字符设备初始化，具体请见dev/char/tty.c*/
  init_sys_task();/*进程系统初始化，具体请见mm/pro/process.c*/
  sti();/*开启中断，请见lib/io.h*/
  move_to_user_mode();/*移动到用户模式（进程1）,具体请见mm/pro/process.h*/
  start_process_disp=1;/*允许时间片进程调度*/
  i=system_call_00(FORK);/*创建进程2*/
  if(!i){
    init_2();/*此处在进程2执行*/
  }
  while(1);
}
