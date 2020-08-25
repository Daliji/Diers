#include<dev/char/monit/monit.h>
#include<lib/str/string.h>
#include"console.h"

/*终端字符缓冲区*/
static char console_buff[1024];

void con_write(char *s)
{
  memset(console_buff,0,1024);
  strcpy(console_buff,s);
  tty_write();
}

void tty_write()
{
  write_string(console_buff);
}

void tty_recov()
{
  con_write("Deer&Diers:-) ");
}

void con_init()
{
  video_init();
  tty_recov();
}

int printk(char *fmt,...)
{
  char *argc=(char *)(&fmt+1);
  int i=vsprintf(fmt,console_buff,argc);
  if(i==-1)return i;
  tty_write();
  return i;
}

/*对齐不正确*/
int put_align_str(char *str,int start,int end,int m,int n)
{
  int tmp_m=m;
  int align=0;
  if(m<0){
    m=-m;
    align=1;
  }
  if(n>m){
    return start+n;
  }
  if(n==DEFAULT_ALIGN_NR){
    if(end-start>=m)return end;
    if(!align){
      for(;end>=start;end--){
	str[start+m--]=str[end];
      }
      for(;m>=0;m--){
	str[start+m]=' ';
      }
    }else{
      for(;end<=start+m;end++){
	str[end]=' ';
      }
    }
    m=tmp_m>0? tmp_m:-tmp_m;
    return start+m;
  }else{
    return put_align_str(str,start,n,tmp_m,DEFAULT_ALIGN_NR);
  }
}

int put_align_integer(char *str,int start,int end,int m,int n)
{
  int tmp_m=m;
  if(m<0)return -1;
  if(n!=DEFAULT_ALIGN_NR)return -1;
  if(m==DEFAULT_ALIGN_NR)return end;
  if(m<end-start)return end;
  m--;
  for(;end>=start;end--){
    str[start+m--]=str[end];
  }
  for(;m>=0;m--){
    str[start+m]=' ';
  }
  return start+tmp_m-1;
}

int put_align_float(char *str,int start,int end,int m,int n)
{
  return -1;
}

/*没考虑小数*/
int _get_str_nr_(char *str,int *i)
{
  int number=0;
  (*i)++;
  if(str[*i]!='-'&&!(is_digit(str[*i]))){
    (*i)--;
    return DEFAULT_ALIGN_NR;
  }
  while(is_digit(str[*i])){
    number=number*10+str[*i]-'0';
    (*i)++;
  }
  (*i)--;
  return number;
}

int _get_number_(char *canshu,int *num,int jinzhi)
{
  int weishu=1;
  int tmp=*((int *)(canshu));
  *num=tmp;
  while(tmp/jinzhi!=0){
    weishu++;
    tmp/=jinzhi;
  }
  return weishu;
}

int vsprintf(char *fmt,char *con_buf,char *argc)
{
  int len=strlen(fmt);
  int argc_count=0;
  int mark_1=0;
  int width=DEFAULT_ALIGN_NR;
  int get_wid=DEFAULT_ALIGN_NR;
  int mark_2=0;
  int i,j=0;
  char c;
  int start,tmp_b,tmp_a,tmp_c;
  for(i=0;i<len;i++){
    c=fmt[i];
    if(mark_1){
      if(c=='%'){
	con_buf[j++]='%';
	mark_1=0;
	mark_2=0;
	width=DEFAULT_ALIGN_NR;
	get_wid=DEFAULT_ALIGN_NR;
      }else if(c=='.'){
	if(mark_2)return -1;
	get_wid=_get_str_nr_(fmt,&i);
	mark_2=1;
      }else{
	switch(c){
	case 'd':
	case 'i':
	LABLE_1:
	  if(get_wid!=DEFAULT_ALIGN_NR)return -1;
	  tmp_b=tmp_c=_get_number_((char *)(argc+argc_count),&tmp_a,10);
	  argc_count+=4;
	  start=j;
	  j+=tmp_b-1;
	  while(tmp_c--){
	    con_buf[j--]=tmp_a%10+'0';
	    tmp_a/=10;
	  }
	  j+=tmp_b;
	  j=put_align_integer(con_buf,start,j,width,get_wid);
	  j++;
	  //复位
	  mark_1=0;
	  mark_2=0;
	  width=DEFAULT_ALIGN_NR;
	  get_wid=DEFAULT_ALIGN_NR;
	  break;
	case 'c':
	  if(get_wid!=DEFAULT_ALIGN_NR||width!=DEFAULT_ALIGN_NR)return -1;
	  con_buf[j++]=*((char *)(argc)+argc_count);
	  argc_count+=4;
	  mark_1=0;
	  mark_2=0;
	  width=DEFAULT_ALIGN_NR;
	  get_wid=DEFAULT_ALIGN_NR;
	  break;
	case 'l':
	  i++;
	  c=fmt[i];
	  if(c=='d'){
	    goto LABLE_1;
	  }else{
	    return -1;
	  }
	  break;
	case 'o':
	  if(get_wid!=DEFAULT_ALIGN_NR)return -1;
	  tmp_b=tmp_c=_get_number_((char *)(argc+argc_count),&tmp_a,8);
	  argc_count+=4;
	  start=j;
	  j+=tmp_b-1;
	  while(tmp_c--){
	    con_buf[j--]=tmp_a%8+'0';
	    tmp_a/=8;
	  }
	  j+=tmp_b;
	  j=put_align_integer(con_buf,start,j,width,get_wid);
	  j++;
	  //复位
	  mark_1=0;
	  mark_2=0;
	  width=DEFAULT_ALIGN_NR;
	  get_wid=DEFAULT_ALIGN_NR;
	  break;
	case 'x':
	  if(get_wid!=DEFAULT_ALIGN_NR)return -1;
	  tmp_b=tmp_c=_get_number_((char *)(argc+argc_count),&tmp_a,16);
	  argc_count+=4;
	  start=j;
	  j+=tmp_b-1;
	  while(tmp_c--){
	    con_buf[j--]=tmp_a%16+'0';
	    if(con_buf[j+1]>'9'){
	      con_buf[j+1]='a'+con_buf[j+1]-'9'-1;
	    }
	    tmp_a/=16;
	  }
	  j+=tmp_b;
	  j=put_align_integer(con_buf,start,j,width,get_wid);
	  j++;
	  //复位
	  mark_1=0;
	  mark_2=0;
	  width=DEFAULT_ALIGN_NR;
	  get_wid=DEFAULT_ALIGN_NR;
	  break;
	case 'X':
	  if(get_wid!=DEFAULT_ALIGN_NR)return -1;
	  tmp_b=tmp_c=_get_number_((char *)(argc+argc_count),&tmp_a,16);
	  argc_count+=4;
	  start=j;
	  j+=tmp_b-1;
	  while(tmp_c--){
	    con_buf[j--]=tmp_a%16+'0';
	    if(con_buf[j+1]>'9'){
	      con_buf[j+1]='A'+con_buf[j+1]-'9'-1;
	    }
	    tmp_a/=16;
	  }
	  j+=tmp_b;
	  j=put_align_integer(con_buf,start,j,width,get_wid);
	  j++;
	  //复位
	  mark_1=0;
	  mark_2=0;
	  width=DEFAULT_ALIGN_NR;
	  get_wid=DEFAULT_ALIGN_NR;
	  break;
	case 'u':
	  if(get_wid!=DEFAULT_ALIGN_NR)return -1;
	  tmp_b=tmp_c=_get_number_((char *)(argc+argc_count),&tmp_a,10);
	  argc_count+=4;
	  start=j;
	  j+=tmp_b-1;
	  while(tmp_c--){
	    con_buf[j--]=tmp_a%10+'0';
	    tmp_a/=10;
	  }
	  j+=tmp_b;
	  j=put_align_integer(con_buf,start,j,width,get_wid);
	  j++;
	  //复位
	  mark_1=0;
	  mark_2=0;
	  width=DEFAULT_ALIGN_NR;
	  get_wid=DEFAULT_ALIGN_NR;
	  break;
	case 's':
	  for(tmp_a=0;;tmp_a++){
	    c=*((char *)(*((int *)(argc+argc_count)))+tmp_a);
	    if(c=='\0')break;
	    else con_buf[j++]=c;
	  }
	  argc_count+=4;
	  mark_1=0;
	  mark_2=0;
	  width=get_wid=DEFAULT_ALIGN_NR;
	  break;
	case 'f':
	case 'e':
	case 'E':
	case 'g':
	case 'G':
	default:return -1;
	}
      }
    }else if(c=='%'){
      mark_1=1;
      width=_get_str_nr_(fmt,&i);
    }else{
      con_buf[j++]=c;
    }
  }
  con_buf[j]='\0';
  return strlen(con_buf);
}
