 
#define DEFAULT_ALIGN_NR 9999
/*写终端函数*/
void con_write(char *);
/*向终端写入缓冲区函数*/
void tty_write();
/*向终端写入提示符*/
void tty_recov();
/*终端初始化函数*/
void con_init();
/*规范字符串转换*/
int vsprintf(char *fmt,char *con_buf,char *argc);
/*内核规范打印函数*/
int printk(char *fmt,...);
/*字符串对齐函数*/
int put_align_str(char *str,int start,int end,int m,int n);
/*整数对齐函数*/
int put_align_integer(char *str,int start,int end,int m,int n);
/*小数对齐函数*/
int put_align_float(char *str,int start,int ,int ,int);
/*从字符串中得到数字*/
int _get_str_nr_(char *,int *);
/*按照进制取得数字，返回位数*/
int _get_number_(char *,int *,int);

#define is_digit(a) ((a)>='0'&&(a)<='9')
