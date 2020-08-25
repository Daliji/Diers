#include<int/int.h>

#define MEM_SIZE (64*1024*1024)
/*页表开始处*/
#define PAGE_TABLE_ 0x00100000
/*缓冲区开始处*/
#define MEM_BUFF_START 0x00500000
/*用户内存开始处*/
#define MEM_START 0x02000000
/*页目录表开始处*/
#define PAGE_TABLE_DIR_ 0x00001000
/*总页数*/
#define PAGE_NUMBER (MEM_SIZE>>12)
/*系统内存页数*/
#define SYS_PAGE_NUMBER (MEM_START>>12)
/*根据线性地址得到某个页表的基址*/
#define get_pagetab_addr(address) \
  (((unsigned int)*(0x1000+((address)>>22<<2)))&0xfffff000)
/*根据线性地址得到某页的基址*/
#define get_page_addr(address) \
  ((*(get_pagetab_addr(address))+((address)&0x003ff000)>>12<<2)&0xfffff000)
/*由线性地址得到物理地址*/
#define get_phy_addr(address) \
  (get_page_addr(address)+(address)&0x00000fff)
/*从段描述符里面取得段基址*/
#define get_segbase(a,b) \
  ((((a)>>16)&0xffff)|(((b)&0xff)<<16)|((b)&0xff000000))
/*取得基址的下标*/
#define get_nr(a,b) ({				\
      unsigned int X=get_segbase((a),(b));	\
      X=1024*(X>>22)+((X>>12)&0x3ff);		\
      X;					\
    })
/*根据下标设置段基址*/
#define set_segbase(_base,a,b) ({		\
      (b)=(((b)&0x00ffff00)|((_base)&0xff000000)|(((_base)&0x00ff0000)>>16)); \
      (a)=((a)&0x0000ffff)|(((_base)&0x0000ffff)<<16);			\
    })

/*GDT地址开始处*/
#define GDT_START 0x3000
/*LDT开始处*/
#define LDT_START GDT_START+32
/*TSS开始处*/
#define TSS_START GDT_START+24

#define set_tss_d(n,addr) set_tss_desc(TSS_START+16*(n),(addr))
#define set_ldt_d(n,addr) set_ldt_desc(LDT_START+16*(n),(addr))

#define set_tss_desc(n,addr) \
  _set_tssldt_desc((n),(addr),0x89)
#define set_ldt_desc(n,addr) \
  _set_tssldt_desc((n),(addr),0x82)

#define _set_tssldt_desc(desc_addr,addr,type) \
  (*(unsigned int *)(desc_addr))=((((addr)&0x0000ffff)<<16)|104);    \
  (*((unsigned int *)(desc_addr)+1))=(((addr)&0xff0000)>>16)|((unsigned int)(type)<<8&0x0000ff00|((addr)&0xff000000))

/*LDT在GDT中的偏移*/
#define _LDT(n) ((n)*16+32)
/*TSS在GDT中的偏移*/
#define _TSS(n) ((n)*16+24)
/*加载LDTR寄存器*/
#define lldt(n) __asm__("lldt %%ax"::"a"(_LDT(n)))
/*加载TR寄存器*/
#define ltr(n) __asm__("ltr %%ax"::"a"(_TSS(n)))

/*初始化内存页表*/
void init_memory();

unsigned int get_free_pages(int n);

/*取得从n开始空白内存块页数大小*/
unsigned int get_free_block_nr(int *n);

/*复制内存页表*/
void copy_pagetables(int oldsegbase,int newsegbase,int n);

/*复制页面，复制一页内存*/
#define copy_page(from,to) \
  __asm__("cld ; rep ; movsl"::"S"(from),"D"(to),"c"(1024))
/*设置mem_mark*/
void set_mem_mark(unsigned int start,unsigned int end,int x);
/*检查n个空的连续的页目录表项*/
int get_free_page_dirs(int n);
/*标记页目录项*/
void markup_pagedirs(int start,int n);
/*申请n个非连续页面来存放页表并且建立映射,pagedir为申请的页目录表开始地方*/
int get_pagetables(int n,int pagedir);
/*取得一页空白内存,返回下标*/
int get_free_page();
/*根据原线性地址下标设置一个页表内容（包括了复制内存）*/
int set_pagetable(int line_addr,int n);
/*页读写保护异常处理*/
int do_page_rw_fault(int line_addr);
/*刷新高速缓存*/
#define invalidate() \
  __asm__("movl %%eax,%%cr3"::"a"(0x1000))
/*将线性地址映射到物理内存*/
void line_to_phy(int lineaddr,int n);
/*缺页异常处理程序*/
int do_no_page_fault(int line_addr);
/*取得一个线性地址对应的页表内容*/
int get_pagetable(int lineaddr);
