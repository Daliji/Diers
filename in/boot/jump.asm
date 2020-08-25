	[BITS 32]
	org	0x0
set_cr0:
	mov	eax,cr0
	and	eax,0x7fffffff
	mov	cr0,eax
	jmp	start
start:
	mov	ax,0x10
	mov	ds,ax

show:
	mov	al,[0x80003]
	mov	ah,[0x80004]
	mov	[0xb8000],al
	mov	[0xb8001],ah
	inc	al
	inc	ah
	mov	[0x80003],al
	mov	[0x80004],ah
	
	jmp	0x8:0x12000
	times	512-($-$$)	db	0