	[bits 32]
	[global keyboard_interrupt]
	[extern keyboard]

keyboard_interrupt:
	push	esi
	push	ebx
	push	ecx
	push	edx
	push	eax
	push	ebp
	push	ds
	push	es
	push	fs
	mov	bx,0x10
	mov	ds,bx
	mov	es,bx
	mov	bx,0x17
	mov	fs,bx
	call	keyboard
	pop	fs
	pop	es
	pop	ds
	pop	ebp
	pop	eax
	pop	edx
	pop	ecx
	pop	ebx
	pop	esi
	iret