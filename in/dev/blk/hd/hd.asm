	[bits 32]
	[extern hd_intr]
	[global _hd_interrupt]

_hd_interrupt:
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	ds
	push	es
	push	fs
	mov	eax,0x10
	mov	ds,ax
	mov	es,ax
	mov	eax,0x17
	mov	fs,ax
	mov	al,0x20
	out	0xa0,al
	xor	edx,edx
	mov	edx,[hd_intr]
	cmp	edx,0
	je	end
	out	0x20,al
	call	[edx]
end:
	pop	fs
	pop	es
	pop	ds
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
ret_hd:	
	iret