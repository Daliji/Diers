	[bits 32]
	[global sys_fork]
	[extern get_free_pid]
	[extern create_new_process]
	[extern show_number]

sys_fork:
	;; 原语，关中断
	cli
	call	get_free_pid
	cmp	eax,0
	je	sys_fork_end
	push	gs
	push	eax
	push	ebx
	call	create_new_process
	add	esp,12
sys_fork_end:
	;; 原语，开中断
	sti
	ret