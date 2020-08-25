
/*
 *tty缓冲队列部分数据结构
 */
typedef struct tty_queue{
  int start;
  int end;
  int threadcount;
  char buf[1024];
}tty_queue;

typedef struct termios{
  unsigned long c_iflag;
  unsigned long c_oflag;
  /*
   *控制字段，第0位控制是否回显 0:yes 1:no
   *第1位控制发送设置 0:发送至终端，1:发送至串口
   *第2位控制插入/替换模式 0:插入模式 1:替换模式
   *第3位控制缓冲模式 0:需要缓冲 1：不需要缓冲
   *第4和5位控制字符处理方式 00:命令行式，无需弹起，只回显普通字符，发送所有字符。
   *                     01:命令行式，无需弹起，回显所有字符，发送所有字符。
   *                     10：特殊式，需要弹起
   */
  unsigned long c_cflag;
}termios;

typedef struct tty_struct{
  termios ter;
  int pid;
  int stoped;
  tty_queue read_q;
  tty_queue write_q;
  tty_queue secondary;
}tty_struct;

/*加入缓冲队列函数*/
void put_queue(char x);
/*标准化处理*/
void deal_buf();
/*字符进入处理*/
void do_char(char x);
/*设置tty模式*/
void set_tty_ctrl(int);
/**/
void deal_char(char c);

/*C语言字符接收函数*/
char not_wait_getchar();


/*贪吃蛇测试*/
void test_snack();
