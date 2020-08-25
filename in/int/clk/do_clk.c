#include<mm/pro/process.h>
#include<dev/char/console.h>

extern unsigned long sys_tick;
extern PCB all_PCB[];
extern char start_process_disp;

void do_timer(unsigned int cpl)
{
  //  if(start_process_disp==1)printk("++++++%d++++++\n",cpl);
  sys_tick%=(2<<30);
  if(sys_tick%1000==0){
    refresh_priority();//刷新优先级
  }
  if(sys_tick%10000==0){
    refresh_memory();//刷新内存
  }
  if(cpl==0)return;
  if(1==start_process_disp)process_disp();
}
