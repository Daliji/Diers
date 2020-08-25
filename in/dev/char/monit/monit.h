
/*当前显示页面*/
#define CVGA_PAGE (*((unsigned short *)(0x80005)))
/*当前显示模式*/
#define CVGA_MODE (*((unsigned char *)(0x80007)))
/*当前显示字符列数*/
#define CVGA_COLUMN_NUMBER (*((unsigned char *)(0x80008)))
/*显示内存大小*/
#define CVGA_MEM_SIZE (64*((*((unsigned char *)(0x80009)))+1))
/*显示状态*/
#define CVGA_STATE (*((unsigned char *)(0x8000a)))
/*显示卡特性参数*/
#define CVGA_T_C (*((unsigned short *)(0x8000b)))

/*显卡初始化*/
void video_init();
/*在制定的地方显示光标*/
void set_show_cur();
/*将光标定位到特定位置*/
void gotoxy(unsigned long x,unsigned long y);
/*写入一个字符（显示一个字符）*/
void write_char(char c);
/*写入一个字符串（显示一个字符串）*/
void write_string(char *s);
/*
 *滚屏操作
*/
/*滚动到特定位置*/
void roll_pos();
/*向上滚动一行*/
void roll_up_oneline();
/*向下滚动一行*/
void roll_down_oneline();
/*向上滚动一页*/
void roll_up_onepage();
/*向下滚动一页*/
void roll_down_onepage();
