	[BITS 32]
	[global show_msg_]
	[extern e_main]
	jmp	start

	_PAGEDIR	equ	0x00001000
	_PAGES	equ	0x00100000
	INT_DESC_TABLE	equ	0x00002000
	GLOBAL_DESC_TABLE	equ	0x00003000

_idt:
	dw	0x0800
	dw	0x2000
	dw	0x0000
start:
	mov	ax,0x10
	mov	ds,ax
	mov	es,ax
	mov	ss,ax
set_page_dir:
	mov	esi,0
	mov	eax,0
s1:
	mov	ebx,_PAGES+7
	add	ebx,eax
	mov	[_PAGEDIR+esi],ebx
	add	eax,4096
	add	esi,4
	cmp	esi,4096
	jz	end_set_page_dir
	jmp	s1
end_set_page_dir:
	mov	esi,0
	mov	ecx,0
	mov	ebx,0x00008000
	mov	eax,0x00008e00
set_int_table:
	mov	[INT_DESC_TABLE+esi],ebx
	add	esi,4
	mov	[INT_DESC_TABLE+esi],eax
	add	esi,4
	inc	ecx
	cmp	ecx,512
	jz	set_int_end
	jmp	set_int_table
set_int_end:
	lidt	[_idt]
	mov	eax,0
	mov	ebx,0x07
set_page_table:	
	mov	[_PAGES+eax],ebx
	add	ebx,0x1000
	add	eax,4
	cmp	eax,4096
	jz	end_set_page_table
	jmp	set_page_table
end_set_page_table:
	
open_pages:
set_cr3:
	mov	eax,0x00001000
	mov	cr3,eax
set_cr0:
	mov	eax,cr0
	or	eax,0x80000000
	mov	cr0,eax
open_pages_end:

	call	e_main
	jmp	$

show_msg_:
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	ds
	push	es
	push	ss
	push	fs
	mov	ax,0x10
	mov	ds,ax

	xor	esi,esi
	mov	cl,'a'
	mov	dl,0x04
show:	
	mov	[0x000b8000+esi],cl
	mov	[0x000b8001+esi],dl
	inc 	dl
	inc	esi
	inc	esi
	inc	cl
	cmp	si,10000
	je	end_show
	jmp	show

end_show:
	pop	fs
	pop	ss
	pop	es
	pop	ds
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	ret
