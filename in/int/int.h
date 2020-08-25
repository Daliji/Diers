/*中断向量表开始处*/
#define INT_ADDR 0x00002000
/*数组换算*/
#define INT_ADDR_I(n) INT_ADDR+8*(n)
/*底层设置*/
#define _set_gate(gate_addr,attr,addr) \
  (*((unsigned int *)(gate_addr)))=(0x00080000|((addr)&0x0000ffff));	\
  ((*((unsigned int *)((char *)(gate_addr)+4)))=(attr)|(addr)&0xffff0000)

/*中断门*/
#define set_intr_gate(n,addr) \
  _set_gate(INT_ADDR_I(n),0x8e00,(unsigned int)(addr))
/*陷阱门*/
#define set_trap_gate(n,addr) \
  _set_gate(INT_ADDR_I(n),0x8f00,(unsigned int)(addr))
/*系统调用*/
#define set_system_gate(n,addr) \
  _set_gate(INT_ADDR_I(n),0xef00,(unsigned int)(addr))
