
/*硬盘控制寄存器*/
#define HD_CMD 0x3f6
/*数据寄存器*/
#define HD_DATA 0x1f0
/*错误寄存器*/
#define HD_ERROR 0x1f1
/*扇区数寄存器*/
#define HD_NSECTOR 0x1f2
/*扇区号寄存器*/
#define HD_SECTOR 0x1f3
/*柱面号寄存器低字节*/
#define HD_LCYL 0x1f4
/*柱面号寄存器高字节*/
#define HD_HCYL 0x1f5
/*驱动器/磁头寄存器--驱动器号/磁头号*/
#define HD_CURRENT 0x1f6
/*主状态寄存器*/
#define HD_STATUS 0x1f7
/*命令寄存器*/
#define HD_COMMAND 0x1f7

/*读写错误判错次数*/
#define G_TIME 10

/*最多块设备数*/
#define MAX_BLK 10

/*硬盘设备号*/
#define HD 3

/*读端口port，读取nr字，保存在buf中*/
#define port_read(port,buf,nr) \
  __asm__("cld;rep;insw"::"d"(port),"D"(buf),"c"(nr))
/*写端口port，写入nr字，原数据在buf中*/
#define port_write(port,buf,nr) \
  __asm__("cld;rep;outsw"::"d"(port),"S"(buf),"c"(nr))

/*硬盘参数及类型*/
typedef struct hd_info{
  int head;//磁头数
  int sect;//每磁道扇区数
  int cyl;//柱面数
  int wpcom;//写前预补偿柱面号
  int lzone;//磁头着陆柱面号
  char ctrl;//控制字节
}hd_info;

/*硬盘分区结构*/
typedef struct hd_struct{
  int start_sec;//物理起始扇区号
  int sec_number;//分区扇区数
}hd_struct;

/*块设备请求项*/
typedef struct hd_request{
  int hd_info;//硬盘参数及类型指针
  int hd_struct;//硬盘分区结构指针
  int dev;//驱动器/磁头号
  char cmd;//读/写命令 读：0 写：1
  char errors;//操作产生错误次数
  int start_sec;//起始扇区
  int sec_num;//要读写扇区数
  char *buf;//缓冲区
  struct hd_request *next;//队列下一项指针
}blk_request;

/*命令格式表*/
typedef struct hd_ctrl{
  char ctrl;//硬盘控制寄存器
  char start_cir;//写预补偿起始柱面号
  char sec_num;//扇区数
  char start_sec;//起始扇区号
  char cir_low;//柱面号低字节
  char cir_high;//柱面号高字节
  char dev_num;//驱动器号，磁头号
  char command;//命令码
}hd_ctrl;

/*
 *循环处理硬盘设备请求项
 *返回结果如下：
 *0：正常
 *-1：控制器忙碌
 *-2：驱动器未就绪
 */
int do_hd_request(blk_request *b);

/*
 *判断检测控制器空闲状态 0为空闲，1为忙碌
 */
int check_busy();

/*
 *检测驱动器是否就绪
 *返回结果如下：
 *1:就绪
 *0:未就绪
 */
int check_ready();

/*发送命令控制块*/
void send_ctrl(hd_ctrl *ctrl,void (*hdintr)());

/*读硬盘中断函数*/
void hd_readintr();
/*写硬盘中断函数*/
void hd_writeintr();

/*检测读写是否成功，返回1表示成功，0表示失败*/
int check_op_success();

/*复位硬盘处理*/
void reset_hd();

/*读写错误处理*/
void bad_rw_intr();

/*硬盘初始化函数*/
void hd_init();
