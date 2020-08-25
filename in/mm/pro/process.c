#include"process.h"
#include<int/clk/clk.h>
#include<mm/mm.h>

/*PCB数组 开始于5MB处*/
PCB *all_PCB=(PCB *)(0x500000);
/*正在运行指针*/
PCB_running_pointer *p_running=0;
/*当前PCB数组下标*/
static int curr;
/*当前PCB指针*/
PCB *cur_pcb;
/*最大就绪队列非空pri*/
int max_priority;
/*就绪队列指针*/
PCB_ready_state_pointer *p_ready=0;
/*等待队列指针*/
PCB_wait_state_pointer *p_wait=0;
/*时间片数*/
extern unsigned long sys_tick;

/*测试数字*/
unsigned int X_test;

PCB_running_pointer Prp;
PCB_ready_state_pointer Prsp;
PCB_priority_pointer *Ppp=(PCB_priority_pointer*)(0x800000);//8MB处
PCB_wait_state_pointer Pwsp;
PCB_waitreason_pointer *Pwp=(PCB_waitreason_pointer*)(0x810000);
PCB_priority_pointer *Ppp1=(PCB_priority_pointer*)(0x820000);

ldt_struct ldt_struct_0[3];

void init_sys_task()
{
  int i,j;
  curr=0;
  max_priority=0;
  /*初始化正在运行指针*/
  p_running=&Prp;
  all_PCB[MAX_PCB_NR_1-1].pid=MAX_PCB_NR_1-1;
  all_PCB[MAX_PCB_NR_1-1].p_pri_a=0;
  all_PCB[MAX_PCB_NR_1-1].p_pri_b=0;
  all_PCB[MAX_PCB_NR_1-1].pid=MAX_PCB_NR_1-1;
  Prp.head=&(all_PCB[MAX_PCB_NR_1-1]);
  Prp.tail=Prp.head;
  /*初始化就绪状态指针*/
  p_ready=&Prsp;
  Prsp.head=Ppp;
  for(i=0;i<MAX_PRIORITY;i++){
    all_PCB[MAX_PCB_NR_1-2-i].pid=MAX_PCB_NR_1-2-i;
    all_PCB[MAX_PCB_NR_1-2-i].p_pri_a=0;
    all_PCB[MAX_PCB_NR_1-2-i].p_pri_b=0;
    Ppp[i].head=&(all_PCB[MAX_PCB_NR_1-2-i]);
    Ppp[i].tail=Ppp[i].head;
  }
  /*初始化阻塞队列指针*/
  p_wait=&Pwsp;
  Pwsp.head=Pwp;
  for(i=0;i<MAX_WAIT_REASON;i++){
    Pwp[i].head=&(Ppp1[i*MAX_PRIORITY]);
    for(j=0;j<MAX_PRIORITY;j++){
      all_PCB[MAX_PCB_NR+1+j].pid=MAX_PCB_NR+1+j;
      all_PCB[MAX_PCB_NR+1+j].p_pri_a=0;
      all_PCB[MAX_PCB_NR+1+j].p_pri_b=0;
      Ppp[j].head=&(all_PCB[MAX_PCB_NR+1+j]);
      Ppp[j].tail=Ppp[j].head;
    }
  }
  //设置所有pid
  for(j=0;j<MAX_PCB_NR_1;j++){
    all_PCB[j].pid=j;
  }
  for(j=2;j<MAX_PCB_NR_1;j++){
    all_PCB[j].pstate=-1;
  }
  /*设置当前任务指针*/
  cur_pcb=&(all_PCB[1]);
  /*初始化1号进程*/
  init_1();
  /*插入就绪队列*/
  //  add_to_queue(PRI_QUEUE_READY,*cur_pcb);
  /*设置tss/ldt*/
  set_tss_d(0,(int)(&(cur_pcb->p_tss)));
  set_ldt_d(0,(int)(cur_pcb->p_ldt));
  /*复位NT标志*/
  __asm__("pushfl\n\t"
	  "andl $0xffffbfff,(%esp)\n\t"
	  "popfl");
  /*加载TR/LDTR寄存器*/
  lldt(0);
  ltr(0);
  cur_pcb=0;
  curr=1;
  /*初始化定时器*/
  clk_init();
  X_test=0;
}

void refresh_priority()
{
}

void refresh_memory()
{
}

int get_free_pid()
{
  int pid=0;
  int j=0;
  int x=0;
  for(j=1;j<MAX_PCB_NR;j++){
    if(all_PCB[j].pstate==-1){
      pid=j;
      break;
    }else{
      x=j;
    }
  }
  __asm__("movl %0,%%ebx"::"g"(x));
  return pid;
}
/*是否设置成原语？？？*/
int create_new_process(int lastpid,int pid,int gs,int none1,int fs,	\
		       int es,int ds,int ebp,int edi,int edx,	\
		       int ecx,int ebx,int esi,int eip,\
		       int cs,int eflags,int esp,int ss)
{
  int i;
  unsigned int __limit;//段限
  unsigned int __page;//mem_mark下标
  unsigned int __pagedir;
  unsigned int __pagedir_num;
  unsigned int tmp;
  unsigned int new_base;
#define np all_PCB[pid]
  //设置ldt
  np.p_ldt[0].a=0;
  np.p_ldt[0].b=0;
  np.p_ldt[1].a=all_PCB[curr].p_ldt[1].a;
  np.p_ldt[1].b=all_PCB[curr].p_ldt[1].b;
  np.p_ldt[2].a=all_PCB[curr].p_ldt[2].a;
  np.p_ldt[2].b=all_PCB[curr].p_ldt[2].b;
  //取得段限
  __limit=np.p_ldt[1].a&0xffff;
  //申请页目录项
  __pagedir_num=(__limit*4*1024+4*1024*1024-1)/(4*1024*1024);
  __pagedir=get_free_page_dirs(__pagedir_num);
  if(__pagedir==-1)return -1;
  //申请存放页表位置并建立映射
  if(get_pagetables(__pagedir_num,__pagedir)<0)return -1;
  //标记页目录项
  markup_pagedirs(__pagedir,__pagedir_num);
  //重新设置段基址
  new_base=__pagedir<<22;//取得新段基址
  set_segbase(new_base,np.p_ldt[1].a,np.p_ldt[1].b);
  set_segbase(new_base,np.p_ldt[2].a,np.p_ldt[2].b);
  //复制页表
  copy_pagetables(get_segbase(all_PCB[curr].p_ldt[1].a,all_PCB[curr].p_ldt[1].b),get_segbase(np.p_ldt[1].a,np.p_ldt[1].b),__pagedir_num);
  invalidate();//一定要刷新!!!
  /*
  __asm__("L:\t"
	  "movl $L,%eax\n\t"
	  "pushl %eax\n\t"
	  "call show_number\n\t");
  while(1);
  */
  //建立链表
  np.back=all_PCB[lastpid].back;
  all_PCB[lastpid].back=pid;
  //父进程
  np.fpid=curr;
  //无子进程
  np.lpid=0;
  //无右兄弟进程
  np.rpid=0;
  //建立父进程指针
  if(all_PCB[curr].lpid==0){
    all_PCB[curr].lpid=pid;
  }else{
    i=all_PCB[curr].lpid;
    while(all_PCB[i].rpid){
      i=all_PCB[i].rpid;
    }
    all_PCB[i].rpid=pid;
  }
  //设置外部标识符
  np.epid=all_PCB[curr].epid;
  //设置用户标识
  np.userid=all_PCB[curr].userid;
  //设置进程优先级
  np.pri=0;
  max_priority=0;
  np.tty=all_PCB[curr].tty;
  //加入就绪队列
  add_to_queue(PRI_QUEUE_READY,np);
  np.pstate=1;
  //设置时间
  np.start_time=0;
  np.all_run_time=0;
  np.last_start_time=0;
  np.all_wait_time=0;
  np.last_over_time=0;
  np.assigned_time=all_PCB[curr].assigned_time;
  //设置tss///////////////
  np.p_tss.fs=fs;
  np.p_tss.es=es;
  np.p_tss.ds=ds;
  np.p_tss.edi=edi;
  np.p_tss.ebp=ebp;
  np.p_tss.edx=edx;
  np.p_tss.ecx=ecx;
  np.p_tss.ebx=ebx;
  np.p_tss.eax=0;//设置子进程返回值
  np.p_tss.esi=esi;
  np.p_tss.cs=cs;
  np.p_tss.eflags=eflags;
  np.p_tss.esp=esp;
  np.p_tss.ss=ss;
  np.p_tss.gs=gs;
  np.p_tss.ldtr=_LDT(pid-1);
  np.p_tss.t=0x80000000;
  np.p_tss.lselector=0x0;
  np.p_tss.esp0=all_PCB[curr].p_tss.esp0;//?????
  np.p_tss.ss0=0x10;
  np.p_tss.cr3=all_PCB[curr].p_tss.cr3;
  set_tss_d(pid-1,(int)(&(np.p_tss)));
  set_ldt_d(pid-1,(int)(np.p_ldt));
  np.p_tss.eip=eip;
  return pid;
}

void show_number(int a)
{
  char *c=(unsigned char *)0xb8000;
  int i,tmp;
  *c='0';
  c+=2;
  *c='x';
  c+=16;
  for(i=0;i<8;i++){
    tmp=a%16;
    if(tmp<10)tmp+='0';
    else tmp=tmp-10+'a';
    *c=tmp;
    c-=2;
    a>>=4;
  }
}

/*时钟中断调度程序*/
void process_disp()
{
  int tmp;
  if(curr>1){
    all_PCB[curr].assigned_time--;
    all_PCB[curr].all_run_time++;
    if(all_PCB[curr].assigned_time>0){
      //something done?
      return;
    }else{
      if(all_PCB[curr].pri<MAX_PRIORITY){
	all_PCB[curr].pri++;
	all_PCB[curr].assigned_time=get_time_pri(all_PCB[curr].pri);
      }
      all_PCB[curr].last_over_time=sys_tick;
      running_ready(all_PCB[curr]);
      if(all_PCB[curr].pri==max_priority+1){
	if(!(p_ready->head[max_priority].head->p_pri_b)){
	  max_priority++;
	}
      }
    }
  }
  tmp=curr;
  curr=p_ready->head[max_priority].head->p_pri_b;//Yes!
  printk("***%d***",curr);
  if(!curr)return;
  ready_running(all_PCB[curr]);
  all_PCB[curr].last_start_time=sys_tick;
  all_PCB[curr].all_wait_time+=(sys_tick-all_PCB[curr].last_over_time);
  X_test++;
  if(tmp==curr)return;
  cur_pcb=&(all_PCB[curr]);
  switch_to(curr);
  printk("|||+++++++++|||");
}

/*自阻塞，未测试*/
void self_wait(int wait_reason)
{
  int tmp;
  all_PCB[curr].wait_reason=wait_reason;
  running_wait(all_PCB[curr]);
  pro_disp();
}

/*通用调度程序*/
void pro_disp()
{
  max_priority=0;
  while(!(p_ready->head[max_priority].head->p_pri_b)){
    max_priority++;
  }
  curr=p_ready->head[max_priority].head->p_pri_b;
  ready_running(all_PCB[curr]);
  cur_pcb=&(all_PCB[curr]);
  switch_to(curr);
}

/*唤醒操作，未测试*/
void wake_up(int wait_reason)
{
  int prio=0;
  int p;
  while(!(p=((((p_wait->head)[wait_reason]).head)[prio]).head->p_pri_b)){
    prio++;
    if(prio>MAX_PRIORITY){
      break;
    }
  }
  if(prio>MAX_PRIORITY)return;
  wait_ready(all_PCB[p]);
}

void init_2();
/*初始化1号进程*/
void init_1()
{
  int temp;
  /*进程标识符*/
  cur_pcb->fpid=0;
  cur_pcb->epid=0;
  cur_pcb->lpid=0;
  cur_pcb->rpid=0;
  cur_pcb->userid=0;
  cur_pcb->exi_code=0;
  /*进程调度信息*/
  cur_pcb->pstate=1;//就绪
  cur_pcb->pri=0;
  cur_pcb->start_time=0;
  cur_pcb->all_run_time=0;
  cur_pcb->last_start_time=0;
  cur_pcb->all_wait_time=0;
  cur_pcb->last_over_time=0;
  cur_pcb->assigned_time=1;
  cur_pcb->back=0;
  /*处理机状态*/
  cur_pcb->tty=0;
  //ldt
  cur_pcb->p_ldt[0].a=0;
  cur_pcb->p_ldt[0].b=0;
  cur_pcb->p_ldt[1].a=0x00001fff;//原1fff现在2000
  cur_pcb->p_ldt[1].b=0x00c0fa00;
  cur_pcb->p_ldt[2].a=0x00001fff;
  cur_pcb->p_ldt[2].b=0x00c0f200;
  //tss
  cur_pcb->p_tss.lselector=0;
  cur_pcb->p_tss.esp0=(int)(&init_2);//???debug.
  cur_pcb->p_tss.ss0=0x10;
  cur_pcb->p_tss.esp1=0;
  cur_pcb->p_tss.ss1=0;
  cur_pcb->p_tss.esp2=0;
  cur_pcb->p_tss.ss2=0;
  cur_pcb->p_tss.cr3=0x1000;
  cur_pcb->p_tss.eip=0;
  __asm__("pushfl;popl %0"::"g"(temp));
  cur_pcb->p_tss.eflags=0;
  cur_pcb->p_tss.eax=0;
  cur_pcb->p_tss.ecx=0;
  cur_pcb->p_tss.edx=0;
  cur_pcb->p_tss.ebx=0;
  cur_pcb->p_tss.esp=0x0;
  cur_pcb->p_tss.ebp=0;
  cur_pcb->p_tss.esi=0;
  cur_pcb->p_tss.es=0x17;
  cur_pcb->p_tss.cs=0x0f;
  cur_pcb->p_tss.ss=0x17;
  cur_pcb->p_tss.ds=0x17;
  cur_pcb->p_tss.fs=0x17;
  cur_pcb->p_tss.gs=0x17;
  cur_pcb->p_tss.ldtr=_LDT(0);
  (cur_pcb->p_tss).t=0x80000000;
}
