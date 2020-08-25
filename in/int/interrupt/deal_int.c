#include<dev/char/console.h>
#include<mm/mm.h>
#include<mm/pro/process.h>
/*int 0*/
void b_dividederror()
{
  printk("int 0x00:interrupt of dividederror!\n");
}

/*int 1*/
void b_debug()
{
  printk("int 0x01:interrupt of debug!\n");
}
/*int 2*/
void b_nmi()
{
  printk("int 0x02:interrupr of NMI interrupt!\n");
}
/*int 3*/
void b_break_point()
{
  printk("int 0x03:interrupt of break point!\n");
}
/*int 4*/
void b_overflow()
{
  printk("int 0x04:interrupt of overflow!\n");
}
/*int 5*/
void b_bondre()
{
  printk("int 0x05:interrupt of Bound Range Exceeded!\n");
}
/*int 6*/
void b_invalid_opcode()
{
  printk("int 0x06:interrupt of Invalid Opcode!\n");
}
/*int 7*/
void b_device_not_avaliable()
{
  printk("int 0x07:interrupt of Device not avaliable!\n");
}
/*int 8*/
void b_double_fault(unsigned int errorcode)
{
  printk("int 0x08:interrupt of Double fault!\n");
  printk("The error code is :%x\n",errorcode);
}
/*int 9*/
void b_cso()
{
  printk("int 0x09:interrupt of Coprocesor segment overrun!\n");
}
/*int 10*/
void b_invalid_tss(unsigned int errorcode)
{
  printk("int 0x0a:interrupt of Invalid TSS!\n");
  printk("The error code is :%x\n",errorcode);
}
/*int 11*/
void b_segment_not_present(unsigned int errorcode)
{
  printk("int 0x0b:interrupt of Segment not present!\n");
  printk("The error code is :%x\n",errorcode);
}
/*int 12*/
void b_ss_fault(unsigned int errorcode)
{
  printk("int 0x0c:interrupt of Stack segment fault!\n");
  printk("The error code is :%x\n",errorcode);
}
/*int 13*/
void b_general_protection(int errorcode)
{
  printk("int 0x0d:interrupt of General protection!\n");
  printk("The error code is :0x%x\n",errorcode);
  while(1);
}
/*int 14*/
void b_page_fault(unsigned int errorcode)
{
  unsigned int line_addr;
  /*
  __asm__("L_1:\t"
	  "movl $L_1,%eax\n\t"
	  "pushl %eax\n\t"
	  "call show_number\n\t");
  while(1);
  */
  /*
  printk("int 0x0e:interrupt of Page fault!\n");
  printk("The error code is :0x%x\n",errorcode);
  */
  __asm__("movl %%cr2,%%eax\n\t"
	  "movl %%eax,%0\n\t"
	  ::"g"(line_addr));
  
  //printk("The line address is :0x%x\n",line_addr);
  //printk("%x ",errorcode);
  if(!(errorcode&0x01)){
    do_no_page_fault(line_addr);
  }else{
    do_page_rw_fault(line_addr);
  }
}
/*int 15*/
void b_reserved()
{
  printk("int 0x0f:interrupt Reserved!\n");
}
/*int 16*/
void b_floating_point_error()
{
  printk("int 0x010:interrupt of Floating pointer error!\n");
}
/*int 17*/
void b_alignment_check(unsigned int errorcode)
{
  printk("int 0x11:interrupt of Alignment check!\n");
  printk("The error code is :%x\n",errorcode);
}
/*int 18*/
void b_machine_check()
{
  printk("int 0x12:interrupt of Machine check!\n");
}
/*int 19*/
void b_sse()
{
  printk("int 0x13:interrupt of Streaming SIMD Extensions!\n");
}

static int ccc_1=0;
void s_str_pro_2()
{
  printk("This is process A!\n");
  /*
  ccc_1++;
  if(ccc_1==50){
    while(ccc_1){
      printk("\b");
      ccc_1--;
    }
  }
  */
}

static int ccc_2=0;
void s_str_pro_3()
{
  printk("this is process B!\n");
  /*
  ccc_2++;
  if(ccc_2==50){
    while(ccc_2){
      printk("\b");
      ccc_2--;
    }
  }
  */
}

extern PCB *cur_pcb;
void print_pcb_info()
{
  printk("******process information******\n");
  printk("pid:%d\n",cur_pcb->pid);
  printk("priority:%d\n",cur_pcb->pri);
  printk("*******************************\n");
}
