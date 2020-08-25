#define outb(value,port) \
  __asm__("outb %%al,%%dx"::"a"(value),"d"(port))

#define inb(port) ({\
  unsigned char v;\
  __asm__ volatile("inb %%dx,%%al":"=a"(v):"d"(port));	\
  v;\
    })

#define outb_p(value,port) \
  __asm__ volatile("outb %%al,%%dx\n"\
		   "\tjmp 1f\n"\
		   "1:\tjmp 1f\n"\
		   "1:"::"a"(value),"d"(port))

#define inb_p(port) ({\
  unsigned char v;\
  __asm__ volatile("inb %%dx,%%al\n"\
		   "\tjmp 1f\n"\
		   "1:\tjmp 1f\n"\
		   "1:":"=a"(v):"d"(port));\
  v;\
})

/*发送中断结束标志eoi*/
#define send_eoi() ({				\
      __asm__("movb $0x20,%al;outb %al,$0x20");	\
    })

#define cli() __asm__("cli");
#define sti() __asm__("sti");

#define FORK 0
#define SHOW_STRING_2 1
#define SHOW_STRING_3 2
#define system_call_00(n) ({			\
      int __res;				\
      __asm__("int $0x80":"=a"(__res):"a"(n));	\
      __res;					\
    })
/*
#define system_call_01(n,value1) ({		\
      int __res;				\
      __asm__("int $0x80":"=a"(__res):"b"((long)(value1)));	\
      __res;							\
    })

#define system_call_02(n,value1,value2) ({	\
      int __res;				\
      __asm__("int $0x80":"=a"(__res):"b"((long)(value1)),"c"((long)(value2))); \
      __res;								\
    })
*/
