#include"string.h"

unsigned int strlen(char *s)
{
  unsigned int i;
  while(i++){
    if(s[i]=='\0')return i;
  }
  return 0;
}

void memset(void *s,char val,int size)
{
  __asm__("cld\n\t"
	  "rep\n\t"
	  "stosb\n\t"
	  ::"a"(val),"D"(s),"c"(size));
}

char * strcpy(char *dest,const char *src)
{
  __asm__("cld\n\t"
	  "1:\tlodsb\n\t"
	  "stosb\n\t"
	  "testb %%al,%%al\n\t"
	  "jne 1b\n\t"
	  ::"S"(src),"D"(dest));
  return dest;
}
