#include<stdio.h>
#include<stdlib.h>

int main()
{
	FILE *fp=NULL;
	FILE *fp1=NULL;
	char s[100];
	char s1[100];
	char c;
	int mark=0;
	long pos;
	printf("please enter the name of file:");
	scanf("%s",s);
	fp=fopen(s,"rb");
	if(NULL==fp)
	{
		printf("Have no such file--01!\n");
		return 0;
	}
	printf("please enter the image file name:");
	scanf("%s",s1);
	fp1=fopen(s1,"rb+");
	if(NULL==fp1)
	{
		printf("Have no such file--02!\n");
		return 0;
	}
	printf("please input the position of the file you want to write:");
	scanf("%ld",&pos);
	fseek(fp1,pos,0);

	while(!feof(fp))
	{
		if(mark)
		{
			fputc(c,fp1);
//			printf("%x  ",c);
		}
		c=fgetc(fp);
		mark=1;
	}
	c=240;
	fputc(c,fp1);
	c=255;
	fputc(c,fp1);
	fputc(c,fp1);
	fclose(fp);
	fclose(fp1);
	printf("write ok!\n");

	return 0;
}
