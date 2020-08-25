#include<mm/pro/process.h>
#include"mm.h"

/*页目录表*/
unsigned int* page_table_dir_i=0;
/*页表*/
unsigned int* pages_i=0;
/*标记内存使用情况数组*/
unsigned char *mem_mark;
/*标记页目录表使用情况*/
unsigned char *memdir_mark;

extern int X_test;

void clear_pagetable()
{
  int i;
  for(i=0;i<SYS_PAGE_NUMBER;i++){
    mem_mark[i]=1;
  }
  for(;i<PAGE_NUMBER;i++){
    mem_mark[i]=0;
  }
  for(i=0;i<8;i++){
    memdir_mark[i]=1;
  }
  for(;i<1024;i++){
    memdir_mark[i]=0;
  }
}

void init_memory()
{
  unsigned int i=0;
  pages_i=(unsigned int *)PAGE_TABLE_;
  page_table_dir_i=(unsigned int *)PAGE_TABLE_DIR_;
  for(i=1024;i<PAGE_NUMBER;i++)
  {
    pages_i[i]=pages_i[i-1]+0x1000;
  }
  mem_mark=(unsigned char *)(0x830000);
  memdir_mark=(unsigned char *)(0x840000);
  clear_pagetable();
}

unsigned int get_free_pages(int n)
{
  unsigned int i=SYS_PAGE_NUMBER;
  unsigned int tmp=0;
  unsigned int tmp1=PAGE_NUMBER-SYS_PAGE_NUMBER;
  int re=0;
  for(;i<PAGE_NUMBER;i++){
    if(!mem_mark[i]){
      tmp=get_free_block_nr(&i);
      if(tmp==n){
	return i-tmp;
      }
      if(tmp>n&&tmp-n<tmp1){
	tmp1=tmp-n;
	re=i-tmp;
      }
    }
  }
  return re;
}

unsigned int get_free_block_nr(int *n)
{
  int re=1;
  while((!mem_mark[(*n)++])&&((*n)<PAGE_NUMBER)){
    re++;
  }
  return re;
}

void copy_pagetables(int oldsegbase,int newsegbase,int n)
{
  int i,j;
  int old_phy_page_addr;
  int new_phy_page_addr;
  int n1,n2;
  for(i=0;i<n;i++){
    //原段基址对应的页表物理地址=内核线性地址
    old_phy_page_addr=page_table_dir_i[(oldsegbase>>22)+i]&0xfffff000;
    //新段基址对应的页表物理地址=内核线性地址
    new_phy_page_addr=page_table_dir_i[(newsegbase>>22)+i]&0xfffff000;
    //保存
    n1=page_table_dir_i[old_phy_page_addr>>22];
    n2=page_table_dir_i[new_phy_page_addr>>22];
    //设置页目录项
    page_table_dir_i[old_phy_page_addr>>22]=PAGE_TABLE_+4096*(old_phy_page_addr>>22)+7;
    page_table_dir_i[new_phy_page_addr>>22]=PAGE_TABLE_+4096*(new_phy_page_addr>>22)+7;
    //复制页表
    copy_page(old_phy_page_addr,new_phy_page_addr);
    //将新的设置为只读
    for(j=0;j<1024;j++){
      (*((unsigned int *)(new_phy_page_addr)+j))&=0xfffffffd;
    }
    //复位
    page_table_dir_i[old_phy_page_addr>>22]=n1;
    page_table_dir_i[new_phy_page_addr>>22]=n2;
  }
}

void set_mem_mark(unsigned int start,unsigned int end,int x)
{
  int i;
  for(i=start;i<end;i++){
    mem_mark[i]=x;
  }
}

int get_free_page_dirs(int n)
{
  int tmp=0;
  int i;
  for(i=4;i<1024;i++){
    if(memdir_mark[i]==0){
      tmp++;
    }
    if(tmp==n){
      return i-tmp+1;
    }
    if(memdir_mark[i]==1){
      tmp=0;
    }
  }
  return -1;
}

void markup_pagedirs(int start,int n)
{
  int i;
  for(i=0;i<n;i++){
    memdir_mark[start+i]++;
  }
}

int get_pagetables(int n,int pagedir)
{
  int i=SYS_PAGE_NUMBER;
  int tmp=0;
  for(;i<PAGE_NUMBER;i++){
    if(mem_mark[i]==0){
      page_table_dir_i[pagedir+tmp++]=i*1024*4+7;
      mem_mark[i]++;
    }
    if(tmp==n){
      return 1;
    }
  }
  //释放,复位
  while(tmp--){
    mem_mark[(page_table_dir_i[pagedir+tmp-1]-7)/(1024*4)]--;
  }
  return -1;
}

int get_free_page()
{
  int i=SYS_PAGE_NUMBER;
  for(;i<PAGE_NUMBER;i++){
    if(mem_mark[i]==0){
      mem_mark[i]=1;
      return i;
    }
  }
  return -1;
}

int set_pagetable(int line_addr,int n)
{
  unsigned int addr1;
  unsigned int x;
  unsigned int tmp;
  if(n<SYS_PAGE_NUMBER)return -1;
  copy_page(line_addr&0xfffff000,0x0);
  addr1=page_table_dir_i[line_addr>>22]&0xfffff000;
  x=addr1>>22;
  tmp=page_table_dir_i[x];
  page_table_dir_i[x]=PAGE_TABLE_+4096*x+0x07;
  *((int *)(addr1+(((line_addr>>12)&0x3ff)*4)))=n*4096+0x07;
  page_table_dir_i[x]=tmp;
  invalidate();
  copy_page(0x0,line_addr&0xfffff000);
  invalidate();
  return 0;
}

int get_pagetable(int lineaddr)
{
  int readdr;
  int x;
  int tmp;
  int addr1=page_table_dir_i[lineaddr>>22]&0xfffff000;
  //  printk("===0x%x===\n",addr1);
  x=addr1>>22;
  tmp=page_table_dir_i[x];
  page_table_dir_i[x]=PAGE_TABLE_+4096*x+0x07;
  readdr=*((int *)(addr1+(((lineaddr>>12)&0x3ff)*4)));
  page_table_dir_i[x]=tmp;
  return readdr;
}

int do_page_rw_fault(int line_addr)
{
  int n;
  n=get_free_page();
  if(n<0)return -1;
  n=set_pagetable(line_addr,n);//
  mem_mark[n]++;
  return n;
}

void line_to_phy(int lineaddr,int n)
{
  int addr1=page_table_dir_i[lineaddr>>22]&0xfffff000;
  int n1=addr1>>22;
  int addr2=page_table_dir_i[n1];
  page_table_dir_i[n1]=4096*n1+PAGE_TABLE_+0x07;
  *((int *)(addr1+4*((lineaddr>>12)&0x3ff)))=4096*n+0x07;
  page_table_dir_i[n1]=addr2;
}

int do_no_page_fault(int line_addr)
{
  int n=get_free_page();
  //  printk("0x%x\n",line_addr);
  //printk("0x%x\n",get_pagetable(line_addr));
  //  while(1);
  if(n<0)return -1;
  line_to_phy(line_addr,n);
  return 1;
}
