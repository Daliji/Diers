	org	0x7c00		;重定位到0x7c00处
	jmp	start		;跳转到start处执行

	SETUPSEG	equ	0x9000 ;setup程序将要加载到的地方
	DATASEG		equ	0x8000 ;数据将要保存的地方
	ROOT_DEV	db	0      ;设备号

start:
	mov	ax,cs
	mov	ds,ax
	mov	es,ax
	mov	ss,ax		;另各段寄存器与CS相等
	mov	[ROOT_DEV],dl	;保存设备号
	call	load_msg	;显示启动信息
	
	mov	ax,DATASEG
	mov	ds,ax
	mov	[0],dl		;将设备号保存到0x80000处
	
	mov	ax,cs
	mov	ds,ax
	
load_setup:			;使用0x13号中断来加载setup程序
	mov	ax,SETUPSEG
	mov	es,ax
	xor	bx,bx		;es:bx指向数据缓冲区
	mov	ax,0x0202	;ah存放功能号，02表示读磁盘到内存，al为读出扇区数量
	mov	dl,[ROOT_DEV]	;dl为驱动器号
	mov	dh,0		;dh为磁头号
	mov	cx,0x0002	;ch为磁道号低8位，cl：开始扇区(0~5)，磁道号高2位(6~7)
	int	0x13		;读取磁盘
	jnc	ok_load_setup	;读写成功则CF复位，否则CF置位
	mov	ax,0		;功能号为0,磁盘复位
	mov	dx,0
	int	0x13		;磁盘复位
	jmp	load_setup
	
ok_load_setup:
	jmp	SETUPSEG:0x0000	;跳转到setup程序运行

load_msg:	
	call	load_curser	;加载光标位置
	mov	ax,loadmessage
	mov	cx,18
	mov	bp,ax
	mov	ax,0x1301
	mov	bx,0x000c
	int	0x10		;显示信息
	ret
	
load_curser:
	mov	ah,0x03
	xor	bh,bh
	int	0x10		;测试加载光标位置
	ret
	
loadmessage	db	"Booting diers..." ;所要显示的信息
		db	13,10		   ;回车换行
	times	510-($-$$)	db	0  ;用0填充510字节内剩下的空间
	dw	0xaa55			   ;引导标志