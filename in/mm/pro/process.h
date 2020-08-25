#include<dev/char/console.h>
#include<dev/char/tty.h>
/*最多阻塞原因*/
#define MAX_WAIT_REASON 20
/*最大优先级*/
#define MAX_PRIORITY 10
/*最大PCB个数，包括空白PCB*/
#define MAX_PCB_NR_1 5000
/*最大非空白PCB数*/
#define MAX_PCB_NR (MAX_PCB_NR_1-1-MAX_PRIORITY-MAX_WAIT_REASON*MAX_PRIORITY)

/*任务数据结构*/
typedef struct tss{
  unsigned int lselector;//0
  unsigned int esp0;//4
  unsigned int ss0;//8
  unsigned int esp1;//12
  unsigned int ss1;//16
  unsigned int esp2;//20
  unsigned int ss2;//24
  unsigned int cr3;//28
  unsigned int eip;//32
  unsigned int eflags;//36
  unsigned int eax;//40
  unsigned int ecx;//44
  unsigned int edx;//48
  unsigned int ebx;//52
  unsigned int esp;//56
  unsigned int ebp;//60
  unsigned int esi;//64
  unsigned int edi;//68
  unsigned int es;//72
  unsigned int cs;//76
  unsigned int ss;//80
  unsigned int ds;//84
  unsigned int fs;//88
  unsigned int gs;//92
  unsigned int ldtr;//96
  unsigned int t;//100
}tss;

/*ldt*/
typedef struct ldt{
  unsigned int a;
  unsigned int b;
}ldt;

/*用户标识*/
typedef struct user{
}user;

/*等待进程资源队列*/
typedef struct wait_queue{
  unsigned int pid;
  struct wait_queue *next;
}wait_queue;

/*定义PCB数据结构*/ 
typedef struct PCB{
  /*进程标识符*/
  unsigned int pid;//内部标识符
  unsigned int fpid;//父进程内部标识符
  char *epid;//外部标识符
  unsigned int lpid;//左儿子
  unsigned int rpid;//右兄弟
  user *userid;//用户标识
  int exi_code;//进程退出代码

  /*进程调度信息*/
  char pstate;//进程状态
  unsigned char pri;//进程优先级
  unsigned int p_pri_a;//指向进程优先级队列链表指针，前
  unsigned int p_pri_b;//指向进程优先级队列链表指针，后
  unsigned long start_time;//进程开始时间
  unsigned long all_run_time;//进程全部运行时间
  unsigned long last_start_time;//进程上次运行时间
  unsigned long all_wait_time;//进程总等待时间
  unsigned long last_over_time;//进程上次结束运行时间
  unsigned long assigned_time;//分配时间片
  unsigned int wait_reason;//事件，进程由执行转为阻塞原因
  // unsigned int pre;//顺序表，前指针
  unsigned int back;//顺序表，后指针

  /*处理机状态*/
  //缓冲
  tty_struct *tty;
  ldt p_ldt[3];
  tss p_tss;

  /*进程控制信息*/

  /*线程相关*/
}PCB;

typedef struct ldt_struct{
  unsigned int a,b;
}ldt_struct;

/*正在运行指针*/
typedef struct PCB_running_pointer{
  PCB *head;//头指针
  PCB *tail;//尾指针
}PCB_running_pointer;

/*优先级指针*/
typedef struct PCB_priority_pointer{
  PCB *head;//头指针
  PCB *tail;//尾指针
}PCB_priority_pointer;

/*就绪指针*/
typedef struct PCB_ready_state_pointer{
  PCB_priority_pointer *head;
}PCB_ready_state_pointer;

/*阻塞原因指针*/
typedef struct PCB_waitreason_pointer{
  PCB_priority_pointer *head;
}PCB_waitreason_pointer;

/*阻塞状态指针*/
typedef struct PCB_wait_state_pointer{
  PCB_waitreason_pointer *head;
}PCB_wait_state_pointer;

/*取得空PID号码*/
int get_free_pid();

/*等待*/
#define PRI_QUEUE_WAIT(p) (((p_wait->head)[(p).wait_reason]).head)[(p).pri]
/*就绪*/
#define PRI_QUEUE_READY(p) ((p_ready->head)[(p).pri])
/*执行*/
#define PRI_QUEUE_RUNNING(p) (*(p_running))

/*从队列中取出 Right???*/
#define out_from_queue(p) \
  all_PCB[(p).p_pri_a].p_pri_b=(p).p_pri_b;	\
  if((p).p_pri_b){	                        \
    all_PCB[(p).p_pri_b].p_pri_a=(p).p_pri_a;	\
  }else{					\
    if((p).pstate==0){				\
      p_running->tail=&(all_PCB[(p).p_pri_a]);	\
    }else if((p).pstate==1){			\
      p_ready->head[(p).pri].tail=&(all_PCB[(p).p_pri_a]);	\
    }else{					\
      p_wait->head[(p).wait_reason].head[(p).pri].tail=&(all_PCB[(p).p_pri_a]);	\
    }						\
  }

/*插入队列*/
#define add_to_queue(X,p) \
  (X(p).tail)->p_pri_b=(p).pid;				\
  (p).p_pri_a=(X(p).tail)->pid;                 	\
  (p).p_pri_b=0;					\
     (X(p).tail)=&(p)

/*进程由执行转为等待*/
#define running_wait(p) \
  out_from_queue(p);	       \
  add_to_queue(PRI_QUEUE_WAIT,p);		\
  (p).pstate=2

/*进程由等待转为就绪*/
#define wait_ready(p) \
  out_from_queue(p);				\
  add_to_queue(PRI_QUEUE_READY,(p));		\
  (p).pstate=1

/*进程由就绪转为执行状态*/
#define ready_running(p) \
  out_from_queue(p);				\
  add_to_queue(PRI_QUEUE_RUNNING,(p));		\
  (p).pstate=0

/*进程由执行转为就绪状态*/
#define running_ready(p) \
  out_from_queue(p);				\
  add_to_queue(PRI_QUEUE_READY,(p));		\
  (p).pstate=1

/*建立内存链表初始化内存*/
void init_sys_task();
/*计算优先级和分配时间片关系*/
#define get_time_pri(pri_1) ({			\
      int pri=(pri_1);				\
      int __val=1;				\
      while((pri)--){				\
	__val*=2;				\
      }						\
      __val;					\
    })

/*原语采用关闭中断方式*/
/*刷新进程优先级*/
void refresh_priority();
/*整理内存*/
void refresh_memory();
/*建立新的PCB,参数传递，那个none1???*/
int create_new_process(int lastpid,int pid,int gs,int none1, \
		       int fs,int es,int ds,int edi,int ebp, \
		       int edx,int ecx,int ebx,int esi,			\
		       int eip,int cs,int eflags,int esp,int ss);

/*时钟中断进程调度*/
void process_disp();
/*自阻塞调度程序*/
void self_wait(int wait_reason);
/*唤醒程序*/
void wake_up(int wait_reason);
/*通用调度程序*/
void pro_disp();

/*初始化*/
void init_1();

/*进程切换代码，调用门*/
/*
#define switch_to(n) ({				\
      struct{unsigned int a;unsigned int b;}_a;	\
      __asm__("movw %%dx,%1\n\t"			\
	      "pushl %1\n\t"				\
	      "call show_number\n\t"			\
	      "popl %0\n\t"				\
	      "ljmp *%0\n\t"				\
	      ::"m"(*(&_a.a)),"m"(*(&_a.b)),"d"(_TSS(n-1)));	\
    })
*/
#define switch_to(n) ({				\
      unsigned short _a[4];				\
      __asm__("movw %%dx,%1\n\t"			\
	      "pushl %1\n\t"				\
	      "call show_number\n\t"			\
	      "popl %0\n\t"				\
	      "ljmp *%0\n\t"				\
	      ::"m"(*(&(_a[0]))),"m"(*(&(_a[2]))),"d"(_TSS(n-1)));	\
    })

/*转到用户模式 iret? ret?*/ 
#define move_to_user_mode() \
  __asm__("movl %%esp,%%eax\n\t"		\
	  "pushl $0x17\n\t"			\
	  "pushl %%eax\n\t"			\
	  "pushfl\n\t"				\
	  "pushl $0x0f\n\t"			\
	  "pushl $1f\n\t"			\
	  "iret\n\t"				\
	  "1:\tmovl $0x17,%%eax\n\t"\
	  "movw %%ax,%%ds\n\t"	    \
	  "movw %%ax,%%es\n\t"	    \
	  "movw %%ax,%%fs\n\t"	    \
	  "movw %%ax,%%gs\n\t"	    \
	  :::"eax")

/*对段进行映射*/
void set_segment();

/*取得段限*/
#define get_limit(segment) ({			\
  unsigned long __limit;\
  __asm__("lsll %1,%0\n\tincl %0":"=r"(__limit):"r"(segment));	\
  __limit;							\
    })


void show_number(int);
