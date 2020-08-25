#include"console.h"
#include<dev/char/keyboard/keyboard.h>
#include<mm/pro/process.h>
#include<lib/str/string.h>
#include<dev/char/monit/monit.h>

extern PCB *cur_pcb;
int char_mark;
char tmp_char;

char key_map[105]={0x0,0x0,'1','2','3','4','5','6','7','8','9','0','-','=',\
		   0x0,'\t','q','w','e','r','t','y','u','i','o','p','[',']',\
		   '\n',0x0,'a','s','d','f','g','h','j','k','l',';','\'',\
		   '`',0x0,'\\','z','x','c','v','b','n','m',',','.','/',0x0,\
		   '*',0x0,' ',0x0,0x0,};

int e=0;//标记
char capslock=0;

void put_queue(char x)
{
  tty_struct *tty;
  if(x&0xff==0xe0){
    e=1;
  }else if(x&0xff==0xe1){
    e=2;
  }else if(e!=0){
    e--;
  }
  tty=cur_pcb->tty;
  tty->read_q.buf[(tty->read_q.end)++]=x;
}

void deal_char(char c)
{
  if(char_mark==1){
  }else{
    deal_buf();
  }
}

void deal_buf()
{
  tty_struct *tty=cur_pcb->tty;
  char c;
  int tmp;
  if(tty==0)return;
  if(e!=0)return;
  tmp=tty->read_q.buf[tty->read_q.start]&0xff;
  if(tmp==0xe0){
    tty->read_q.start++;
    tmp=tty->read_q.buf[tty->read_q.start++]&0xff;
    if(tmp==0x52){//insert
      if(tty->ter.c_cflag&0x04){
	tty->ter.c_cflag&=0xfffffffb;
      }else{
	tty->ter.c_cflag|=0x04;
      }
    }else if(tmp==0x53){//delete
    }else if(tmp==0x5b){//super
    }else if(tmp==0x38){//R-alt
    }else if(tmp==0x5d){//menue
    }else if(tmp==0x1d){//R-ctrl
    }else if(tmp==0x48){//up
    }else if(tmp==0x50){//down
    }else if(tmp==0x4b){//left
    }else if(tmp==0x4d){//right
    }else if(tmp==0x47){//Home
    }else if(tmp==0x4f){//End
    }else if(tmp==0x49){//page up
    }else if(tmp==0x51){//page down
    }else if(tmp&0x80){//弹起
    }else{//其他，未添加
    }
  }else if(tmp==0xe1){//pause
    tty->read_q.start++;
    tty->read_q.start++;
    tty->read_q.start++;
  }
  while(tty->read_q.start!=tty->read_q.end){
    c=tty->read_q.buf[tty->read_q.start++];
    if(!(c&0x80))c=key_map[c];//错误
    if((tty->secondary.end+1)%1024==tty->secondary.start){
      printk("String too long!\n");
      return;
    }else{
      tty->secondary.buf[tty->secondary.end++]=c;
    }
  }
}

void tty_init()
{
  char_mark=0;
  con_init();
  keyboard_init();
}

int cur_c_cflag;

void set_tty_ctrl(int x){
  cur_c_cflag=x;
}

tty_struct * get_new_tty()
{
  tty_struct t;
    memset(t.read_q.buf,0,1024);
    memset(t.write_q.buf,0,1024);
    memset(t.secondary.buf,0,1024);
    //初始化
    t.pid=cur_pcb->pid;
    t.stoped=0;
    t.read_q.start=t.write_q.start=t.secondary.start=0;
    t.read_q.end=t.write_q.end=t.secondary.end=0;
    t.read_q.threadcount=t.write_q.threadcount=t.secondary.threadcount=1;
    t.ter.c_cflag=cur_c_cflag;
    return &t;
}

int is_putong_char(char c)
{
  return 1;
}

void do_char(char c)
{
  tty_struct *tty;
  int cflag;
  if(cur_pcb->tty==0){
    cur_pcb->tty=get_new_tty();
  }
  tty=cur_pcb->tty;
  cflag=tty->ter.c_cflag;
  cflag=(cflag>>4)&0x03;
  if(cflag==0){
    if(c&0x80){
      return;
    }else{
      if(is_putong_char(c)){
	if(!((tty->ter.c_cflag)&0x01)){
	  write_char(key_map[c&0xff]);
	}
      }
    }
  }else if(cflag==1){
    if(c&0x80){
      return;
    }else{
      if(!((tty->ter.c_cflag)&0x01))write_char(key_map[c&0xff]);
    }
  }else if(cflag==2){
    if(!((tty->ter.c_cflag)&0x01))write_char(key_map[c&0xff]);
  }else{
    return;
  }
  ///////////
  if(!(tty->ter.c_cflag&0x08)){
    char_mark=0;
    put_queue(c);
  }else{
    char_mark=1;
    tmp_char=c;
  }
  deal_char(c);
}

char not_wait_getchar()
{
  tty_struct *tty=cur_pcb->tty;
  char tmp;
  if(tty==0)return 0;
  if(tty->ter.c_cflag&0x08){
    if(char_mark==0)return 0;
    else{
      char_mark=0;
      return key_map[tmp_char&0xff];
    }
  }else{
    if(!(tty->secondary.end==tty->secondary.start)){
      return tty->read_q.buf[(tty->read_q.start)++];
    }else{
      return 0;
    }
  }
  return 0;
}

void test_snack()
{
  char snack_char=4;
  int i,j;
  int width=40;
  int length=20;
  int tmp_systick;
  char c;
  int head=2;
  int tmphead;
  int end=0;
  int snack_line[100]={1,1,1};
  int snack_column[100]={1,2,3};
  int direction=3;//方向：0上，1下，2左，3右
  extern unsigned int sys_tick;
  tmp_systick=sys_tick;
  for(int i=0;i<=length;i++){
    if(i==0||i==length){
      gotoxy(i,0);
      for(j=0;j<=width;j++)printk("=");
      printk("\n");
    }else{
      printk("|");
      gotoxy(i,width);
      printk("|\n");
    }
  }
  gotoxy(1,1);
  printk("%c%c%c",snack_char,snack_char,snack_char);
  set_tty_ctrl(0x09);
  while(1){
    if((c=not_wait_getchar())!=0){
      if(c=='w'){
	if(direction!=1)direction=0;
      }else if(c=='s'){
	if(direction!=0)direction=1;
      }else if(c=='a'){
	if(direction!=3)direction=2;
      }else if(c=='d'){
	if(direction!=2)direction=3;
      }
    }
    if(sys_tick-tmp_systick<10)continue;
    else tmp_systick=sys_tick;
    tmphead=head;
    head=(head+1)%100;
    if(direction==0){
      snack_line[head]=snack_line[tmphead]-1;
      snack_column[head]=snack_column[tmphead];
    }else if(direction==1){
      snack_line[head]=snack_line[tmphead]+1;
      snack_column[head]=snack_column[tmphead];
    }else if(direction==2){
      snack_line[head]=snack_line[tmphead];
      snack_column[head]=snack_column[tmphead]-1;
    }else if(direction==3){
      snack_line[head]=snack_line[tmphead];
      snack_column[head]=snack_column[tmphead]+1;
    }
    gotoxy(snack_line[end],snack_column[end]);
    end=(end+1)%100;
    printk(" ");
    gotoxy(snack_line[head],snack_column[head]);
    printk("%c",snack_char);
    if(snack_line[head]>=length||snack_column[head]>=width||
       snack_line[head]<=0||snack_column[head]<=0){
      goto GAME_END;
    }
    for(i=end;;i++){
      i=i%100;
      if(i==head)break;
      if((snack_line[head]==snack_line[i])&&
	 (snack_column[head]==snack_column[i])){
	printk("---%d--%d--%d---",i,head,end);
	goto GAME_END;
      }
    }
  }
 GAME_END:
  gotoxy(length/2,width/2-5);
  printk("Game Over!");
}
