#define HZ 100
#define CLK_HZ 1193180
#define LATCH (CLK_HZ/HZ)

/*发声初始化*/
#define sount_init() \
  outb_p((inb_p(0x61)|0x02)&0xfe,0x61)
/*时钟芯片初始化*/
void clk_init();
/*定时器初始化*/
void clock_init();
/*发声*/
void playsound(int hz,int time);
