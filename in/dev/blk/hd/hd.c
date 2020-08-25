#include<lib/io.h>
#include<int/int.h>
#include<dev/blk.h>
#include"hd.h"

void (*hd_intr)()=0;

blk_request *cur_blk_request=0;

int hd_reset=0;

hd_request *hd_queue=0;//请求项队列
hd_request *CURRENT_HD_REQUEST=0;//当前请求项

hd_info hd_i[MAX_BLK];

int do_hd_request()
{
  hd_ctrl ctrl;
 REPEAT:
  if(hd_queue==0){
    insert_hd_request(CURRENT_HD_REQUEST);
    goto REPEAT;
  }
  if(check_busy())return -1;
  if(!check_ready())return -2;
  ctrl.ctrl=hd_i[CURRENT_HD_REQUEST->hd_info].ctrl;
  ctrl.start_cir=hd_i[CURRENT_HD_REQUEST->hd_info].wpcom;
  if(CURRENT_HD_REQUEST->cmd==0)send_ctrl(&ctrl,&hd_readintr);
  /*
   *cur_blk_request=b;
   *if(check_busy())return -1;//检测控制器空闲状态
   *if(!check_ready())return -2;//检测驱动器是否就绪
   *if(b->cmd==0)send_ctrl(b->ctrl,&hd_readintr);
   *else send_ctrl(b->ctrl,&hd_writeintr);
   */
}

int check_busy()
{
  int tmp=G_TIME;
  while(inb_p(HD_STATUS)&0x80){
    tmp--;
    if(tmp<=0){
      return 1;//忙碌
    }
  }
  return 0;//空闲
}

int check_ready()
{
  int tmp=G_TIME;
  while(inb_p(HD_STATUS)&0x40){
    tmp--;
    if(tmp<=0){
      return 0;//未就绪
    }
  }
  return 1;//就绪
}

void send_ctrl(hd_ctrl *ctrl,void (*hdintr)())
{
  //0x3f6
  hd_intr=hdintr;
  outb_p(HD_CMD,ctrl->ctrl);
  outb_p(HD_ERROR,ctrl->start_cir>>2);
  outb_p(HD_NSECTOR,ctrl->sec_num);
  outb_p(HD_SECTOR,ctrl->start_sec);
  outb_p(HD_LCYL,ctrl->cir_low);
  outb_p(HD_HCYL,ctrl->cir_high);
  outb_p(HD_CURRENT,dev_num);
  outb_p(HD_COMMAND,ctrl->command);
}

int check_op_success()
{
  if(inb_p(HD_STATUS)&0x01)return 0;
  return 1;
}

void hd_readintr()
{
  if(!check_op_success()){
    bad_rw_intr();
    reset_hd();
  }
  port_read(HD_DATA,cur_blk_request->buf,256);
  cur_blk_request->errors=0;
  //不是一次性读完？为什么还要设置并且中断？
  cur_blk_request->buf+=512;
  cur_blk_request->ctrl->sec_num--;
  cur_blk_request->ctrl->start_sec++;
  if(cur_blk_request->ctrl->sec_num){
    hd_intr=&hd_readintr;
    return;
  }
  end_request(1);
  do_hd_request();
}

void hd_writeintr()
{
  if(!check_op_success()){
    bad_rw_intr();
    reset_hd();
  }
  port_write(HD_DATA,cur_blk_request->buf,256);
  cur_blk_request->buf+=512;
  cur_blk_request->errors=0;
  cur_blk_request->ctrl->sec_num--;
  cur_blk_request->ctrl->start_sec++;
  if(cur_blk_request->ctrl->sec_num){
    hd_intr=&hd_writeintr;
    return;
  }
  end_request(1);
  do_hd_request();
}

void reset_hd()
{
  int i;
  if(!hd_reset)return;
  hd_reset=0;
  outb_p(HD_CMD,0x04);
  for(i=0;i<1000;i++);
  outb_p(HD_CMD,cur_blk_request->ctrl->ctrl);
}

void bad_rw_intr()
{
  if(++cur_blk_request->errors>=10){
    printk("bad hardisk operate.\n");
    end_request(0);
  }
  if(cur_blk_request->errors>=3){
    reset=1;
  }
}

void _hd_interrupt();
void hd_init()
{
  set_intr_gate(0x2e,&_hd_interrupt);
  outb(inb_p(0x21)&0xfb,0x21);
  outb(inb_p(0xa1)&0xbf,0xa1);
}
