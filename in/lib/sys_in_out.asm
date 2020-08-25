	[bits 32]
	[global in_sys_call]
	[global ret_from_sys_call]
	[global sys_call]
	[extern sys_tick]
	[extern sys_call_table]
	[extern show_number]

in_sys_call:
	pop	esi
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
	jmp	esi
ret_from_sys_call:
	pop	esi
	;; cr3不变
	pop	fs
	pop	es
	pop	ds
	pop	ebp
	pop	edi
	pop	edx
	pop	ecx
	pop	ebx
	jmp	esi
sys_call:
	push	esi
	push	E1
	jmp	in_sys_call
E1:
	call	[sys_call_table+eax*4] ;eax
	push	E2
	jmp	ret_from_sys_call
E2:
	pop	esi
	iret