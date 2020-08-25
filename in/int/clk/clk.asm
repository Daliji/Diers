	[bits 32]
	[extern do_timer]
	[extern sys_tick]
	[extern process_disp]
	[global clock_int_start]
	[extern show_number]

clock_int_start:
	push	esi
	push	ebx
	push	ecx
	push	edx
	push	edi
	push	ebp
	push	ds
	push	es
	push	fs
	mov	bx,0x10
	mov	ds,bx
	mov	es,bx
	mov	bx,0x17
	mov	fs,bx
clock_int:
	mov	eax,[sys_tick]
	inc	eax
	mov	[sys_tick],eax
	
	mov	al,0x20
	out	0xa0,al
	out	0x20,al

	mov	ax,[esp+40]
	and	eax,0x03
	
	push	eax
	call	do_timer
	pop	eax
	
	pop	fs
	pop	es
	pop	ds
	pop	ebp
	pop	edi
	pop	edx
	pop	ecx
	pop	ebx
	pop	esi
clk_end:
	iret