	[BITS 32]
	[extern b_dividederror]
	[extern b_debug]
	[extern b_nmi]
	[extern b_break_point]
	[extern b_overflow]
	[extern b_bondre]
	[extern b_invalid_opcode]
	[extern b_device_not_avaliable]
	[extern b_double_fault]
	[extern b_cso]
	[extern b_invalid_tss]
	[extern b_segment_not_present]
	[extern b_ss_fault]
	[extern b_general_protection]
	[extern b_page_fault]
	[extern b_reserved]
	[extern b_floating_point_error]
	[extern b_alignment_check]
	[extern b_machine_check]
	[extern b_sse]
	
	[global _divide_error]
	[global _debug]
	[global _nmi]
	[global _break_point]
	[global _overflow]
	[global _bondre]
	[global _invalid_opcode]
	[global _device_not_avaliable]
	[global _double_fault]
	[global _cso]
	[global _invalid_tss]
	[global _segment_not_present]
	[global _ss_fault]
	[global _general_protection]
	[global _page_fault]
	[global _reserved]
	[global _floating_point_error]
	[global _alignment_check]
	[global _machine_check]
	[global _sse]

	;; no error code interrupts
_have_no_error_code:
	xchg	eax,[esp]
	push	ebx
	push	ecx
	push	edx
	push	edi
	push	esi
	push	ebp
	push	ds
	push	es
	push	fs

	mov	bx,0x10
	mov	ds,bx
	mov	es,bx
	mov	fs,bx
	
	call	eax

	pop	fs
	pop	es
	pop	ds
	pop	ebp
	pop	esi
	pop	edi
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax

	iret

	;; the entrence of no error code interrupts.
_00_trap:	
_divide_error:
	push	b_dividederror
	jmp	_have_no_error_code
_01_trap:	
_debug:
	push	b_debug
	jmp	_have_no_error_code
_02_trap:	
_nmi:
	push	b_nmi
	jmp	_have_no_error_code
_03_sys:	
_break_point:
	push	b_break_point
	jmp	_have_no_error_code
_04_sys:	
_overflow:
	push	b_overflow
	jmp	_have_no_error_code
_05_sys:	
_bondre:
	push	b_bondre
	jmp	_have_no_error_code
_06_trap:	
_invalid_opcode:
	push	b_invalid_opcode
	jmp	_have_no_error_code
_07_trap:
_device_not_avaliable:
	push	b_device_not_avaliable
	jmp	_have_no_error_code
_09_trap:
_cso:
	push	b_cso
	jmp	_have_no_error_code
_15_tarp:
_reserved:
	push	b_reserved
	jmp	_have_no_error_code
_16_tarp:
_floating_point_error:
	push	b_floating_point_error
	jmp	_have_no_error_code
_18_trap:
_machine_check:
	push	b_machine_check
	jmp	_have_no_error_code
_19_trap:
_sse:
	push	b_sse
	jmp	_have_no_error_code


_have_error_code:
	xchg	ebx,[esp]
	xchg	eax,[esp+4]
	push	ecx
	push	edx
	push	edi
	push	esi
	push	ebp
	push	ds
	push	es
	push	fs

	mov	cx,0x10
	mov	ds,cx
	mov	es,cx
	mov	fs,cx

	push	eax
	call	ebx
	pop	eax

	pop	fs
	pop	es
	pop	ds
	pop	ebp
	pop	esi
	pop	edi
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax

	iret

_08_trap:
_double_fault:
	push	b_double_fault
	jmp	_have_error_code
_10_trap:
_invalid_tss:
	push	b_invalid_tss
	jmp	_have_error_code
_11_trap:
_segment_not_present:
	push	b_segment_not_present
	jmp	_have_error_code
_12_trap:
_ss_fault:
	push	b_ss_fault
	jmp	_have_error_code
_13_trap:
_general_protection:
	push	b_general_protection
	jmp	_have_error_code
_14_trap:
_page_fault:
	push	b_page_fault
	jmp	_have_error_code
_17_trap:
_alignment_check:
	push	b_alignment_check
	jmp	_have_error_code
