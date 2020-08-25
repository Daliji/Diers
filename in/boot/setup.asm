	org	0x0000
	jmp	start

	SETUPSEG	equ	0x9000
	DATASEG		equ	0x8000
	SETSEG		equ	0x0400
	INT_TABLE	equ	0x0500
gdtaddr:
	dw	0xffff
	dw	0x3000
	dw	0x0000
gdt:
null_table:
	dw	0,0,0,0
system_code_table:
	dw	0xffff
	dw	0x0000
	dw	0x9a00
	dw	0x00c0
system_data_table:
	dw	0xffff
	dw	0x0000
	dw	0x9200
	dw	0x00c0
idtaddr:
	dw	0x0
	dw	0x0
	dw	0x0
	
start:
	call	load_msg
load_int:
	mov	ax,0
	mov	ds,ax
	xor	si,si
	mov	ax,DATASEG
	mov	es,ax
	mov	di,INT_TABLE
	mov	cx,0x400
	rep	movsb
end_load_int:
load_jump:
	mov	ax,DATASEG
	mov	ds,ax
	mov	dl,[0]
	mov	ax,SETUPSEG
	mov	es,ax
	mov	bx,SETSEG
	mov	ah,0x02
	mov	al,1
	mov	dh,0
	mov	cx,0x0004
	int	0x13
	
	jnc	load_set
	mov	ax,0
	mov	dx,[0]
	int	0x13
	jmp	start

load_set:
	mov	ax,0x1200
	mov	es,ax
	xor	bx,bx
	mov	dl,[0]
	mov	ah,0x02
	mov	al,0x40		;64个扇区，32kb
	mov	dh,0
	mov	cx,0x0005
	int	0x13
	
	jnc	ok_load
	mov	ax,0
	mov	dx,[0]
	int	0x13
	jmp	load_set

ok_load:
get_vidio_mode:
	mov	ah,0x0f
	int	0x10
	mov	[5],bx		;当前显示页面
	;; 0x80007:当前显示模式
	;; 0x80008字符列数
	mov	[7],ax
	mov	ah,0x12
	mov	bl,0x10
	int	0x10
	;; 0x80009:显示内存
	;; 0x8000a:显示状态
	mov	[9],bx
	;; 0x8000b:显卡特性参数
	mov	[11],cx
ok_get_vidio:
;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; hadware read here.
	;; get memory size.
	mov	ax,DATASEG
	mov	ds,ax
	mov	ah,0x88
	int	0x15
	mov	[1],ax
	;; set curser show message.
	mov	al,'b'
	mov	[3],al
	mov	al,0x04
	mov	[4],al
;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; open A20.
	call	open_a20
	;; no interrupts allowed.
	cli
	;; mov set file to 0x0000:0x0000.
	call	move_jump
	;; move gdt to 0x00003000
	call	move_gdt
	;; set gdt.
	lgdt	[gdtaddr]
	;; set idt empty.
	lidt	[idtaddr]
	;; init 8259A.
	call	init_8259a
	;; set cr0
	call	set_cr0
	;; jump to set program.
	jmp	0x8:0x0

move_gdt:
	cld
	mov	ax,0x0000
	mov	es,ax
	mov	di,0x3000
	mov	ax,cs
	mov	ds,ax
	mov	si,gdt
	mov	cx,20
	rep	movsw
	ret

load_msg:
	call	load_curser
	mov	ax,cs
	mov	es,ax
	mov	ax,loadmessage
	mov	cx,16
	mov	bp,ax
	mov	ax,0x1301
	mov	bx,0x000c
	int	0x10
	ret
	
load_curser:
	mov	ah,0x03
	xor	bh,bh
	int	0x10
	ret

open_a20:
o:	mov	dx,0x92
	mov	al,02
	out	dx,al
t:	mov	dx,92h
	in	al,dx
	and	al,02
	jz	o
	ret

move_jump:
	cld
	mov	ax,SETUPSEG
	mov	ds,ax
	mov	si,SETSEG
	mov	ax,0x0
	mov	es,ax
	xor	di,di
	mov	cx,256
	rep	movsw
	ret
	
init_8259a:
	mov	al,0x11
	out	0x20,al
	call	io_delay

	out	0xa0,al
	call	io_delay

	mov	al,0x20
	out	0x21,al
	call	io_delay

	mov	al,0x28
	out	0xa1,al
	call	io_delay

	mov	al,0x04
	out	0x21,al
	call	io_delay

	mov	al,0x02
	out	0xa1,al
	call	io_delay

	mov	al,0x01
	out	0x21,al
	call	io_delay

	out	0xa1,al
	call	io_delay

	mov	al,0xff
	out	0x21,al		;关闭主8259A控制
	call	io_delay

	out	0xa1,al		;关闭从8259A控制
	call	io_delay
	ret
	
io_delay:
	mov	bx,bx
	mov	bx,bx
	mov	bx,bx
	ret
	
set_cr0:
	mov	eax,cr0
	or	eax,1
	mov	cr0,eax
	ret
	
loadmessage:
	db	"Setup diers..."
	db	13,10

	times	1024-($-$$)	db	0