#include"monit.h"
#include<lib/io.h>
#include<lib/str/string.h>
/*显示器类型*/
static unsigned char video_type;
/*屏幕文本列数*/
static unsigned long video_num_columns;
/*每行使用的字节数*/
static unsigned long video_size_row;
/*屏幕文本行数*/
static unsigned long video_num_lines;
/*初始显示页面*/
static unsigned char video_start_page;
/*显示内存起始地址*/
static unsigned long video_mem_start;
/*显示内存结束地址*/
static unsigned long video_mem_end;
/*显示控制索引寄存器端口*/
static unsigned short video_port_reg;
/*显示控制数据寄存器端口*/
static unsigned short video_port_val;
/*擦除字符属性与字符*/
static unsigned short video_erase_char;

/*
 *卷屏操作变量
*/
/*滚屏起始内存地址*/
static unsigned long origin;
/*滚屏末端内存地址*/
static unsigned long scr_end;
/*当前光标位置*/
static unsigned long x;
static unsigned long y;
/*顶行行号，底行行号*/
static unsigned long top,bottom;
/*光标索引位置*/
static unsigned long pos;

/*字符属性*/
static unsigned char attr=0x07;
/*显示类型*/
#define VIDEO_TYPE_MDA 0x10//单色文本
#define VIDEO_TYPE_CGA 0x11//CGA显示器
#define VIDEO_TYPE_EGAM 0x20//EGA/VGA单色
#define VIDEO_TYPE_EGAC 0x21//EGA/VGA彩色


void video_init()
{
  unsigned long i,j;
  video_num_columns=CVGA_COLUMN_NUMBER;//文本列数
  video_size_row=video_num_columns*2;//每行字节数
  video_num_lines=25;//显示字符显示行数
  video_start_page=CVGA_PAGE;//当前显示页面
  video_erase_char=0x0720;//擦除字符？？？？？干什么用？？？？？
  if(CVGA_MODE==7){
    video_mem_start=0xb0000;
    video_port_reg=0x3b4;
    video_port_val=0x3b5;
    if(CVGA_STATE!=0x10){
      video_type=VIDEO_TYPE_EGAM;
      video_mem_end=0xb8000;
    }else{
      video_type=VIDEO_TYPE_MDA;
      video_mem_end=0xb2000;
    }
  }else{
    video_mem_start=0xb8000;
    video_port_reg=0x3d4;
    video_port_val=0x3d5;
    if(CVGA_STATE!=0x10){
      video_type=VIDEO_TYPE_EGAC;
      video_mem_end=0xbc000;
    }else{
      video_type=VIDEO_TYPE_CGA;
      video_mem_end=0xba000;
    }
  }
  j=video_mem_end;
  /*清屏*/
  for(i=video_mem_start;i<j;i++){
    if(i%2==0)*((unsigned char *)(i))=0;
    else *((unsigned char *)(i))=0xf2;//白色背景，绿色字体
  }
  /*设置光标属性*/
  outb_p(10,video_port_reg);
  outb_p(0x40,video_port_val);
  outb_p(11,video_port_reg);
  outb_p(0xff,video_port_val);
  top=0;
  /*滚屏设置*/
  origin=0x0;
  scr_end=origin+video_num_lines*video_num_columns;
  //设置光标位置
  gotoxy(0,0);
}

void gotoxy(unsigned long x1,unsigned long y1){
  x=x1;
  y=y1;
  pos=video_num_columns*(top+x)+y;
  set_show_cur();
}

void set_show_cur(){
  cli();
  outb_p(14,video_port_reg);
  outb_p(0xff&(pos>>8),video_port_val);
  outb_p(15,video_port_reg);
  outb_p(0xff&pos,video_port_val);
  sti();
}

void write_char(char c)
{
  if(c==10){
    if((pos+video_num_columns)*2<video_mem_end-video_mem_start)
      pos+=video_num_columns;
    pos-=pos%video_num_columns;
    goto LABLE_CUR_JMP;
  }
  if(c=='\t'){
    if((pos+8)*2<video_mem_end-video_mem_start)pos+=8;
    goto LABLE_CUR_JMP;
  }
  if(c=='\b'){
    if(pos>0)pos-=1;
    goto LABLE_CUR_JMP;
  }
  __asm__("movl %1,%%eax\n\t"
	  "movb %0,%%bl\n\t"
	  "movb %%bl,(%%eax)\n\t"
	  ::"g"(c),"g"(video_mem_start+pos*2));
  if(pos*2<video_mem_end-video_mem_start)pos++;
 LABLE_CUR_JMP:
  set_show_cur();
  /*滚屏*/
  if(pos>=scr_end)roll_up_oneline();
}

void write_string(char *s)
{
  unsigned long i=strlen(s);
  unsigned long j=0;
  for(;j<i;j++){
    write_char(s[j]);
  }
}

void roll_pos()
{
  outb_p(12,video_port_reg);
  outb_p(0xff&(origin>>8),video_port_val);
  outb_p(13,video_port_reg);
  outb_p(origin&0xff,video_port_val);
}

void roll_down_oneline()
{
  if(origin<video_num_columns)return;
  origin-=video_num_columns;
  scr_end-=video_num_columns;
  top--;
  bottom--;
  x--;
  roll_pos();
}

void roll_up_oneline()
{
  if(scr_end+video_mem_start+video_num_columns>video_mem_end)return;
  origin+=video_num_columns;
  scr_end+=video_num_columns;
  top++;
  bottom++;
  x++;
  roll_pos();
}

void roll_up_onepage()
{
  int i=video_num_lines;
  while(i--&&scr_end+video_mem_start+video_num_columns<=video_mem_end){
    roll_up_oneline();
  }
}

void roll_down_onepage()
{
  int i=video_num_lines;
  while(i--&&origin>=video_num_columns){
    roll_down_oneline();
  }
}
