wlc = gcc -O0 -nostdinc -I. -fno-stack-protector -fpack-struct -std=c99 -c
wln = nasm -f elf
tempfiles = out/set.o out/main.o out/mm.o out/interrupt.o \
out/interruptasm.o out/deal_int.o out/clk.o out/clkasm.o \
out/do_clk.o out/processasm.o out/process.o out/sys_in_out.o \
out/monit.o out/string.o out/console.o out/tty.o out/keyboard.o \
out/keyboardasm.o

out/kernel:out/kernel.bin
	objcopy -R .note -R .comment -S -O binary out/kernel.bin out/kernel

out/kernel.bin:$(tempfiles)
	ld -o out/kernel.bin -Ttext 0x12000 -e e_main $(tempfiles)

out/set.o:boot/set.asm
	$(wln) boot/set.asm -o out/set.o
out/main.o:kernel/main.c
	$(wlc) kernel/main.c -o out/main.o
out/mm.o:mm/mm.c
	$(wlc) mm/mm.c -o out/mm.o
out/interrupt.o:int/interrupt/interrupt.c
	$(wlc) int/interrupt/interrupt.c -o out/interrupt.o
out/interruptasm.o:int/interrupt/interrupt.asm
	$(wln) int/interrupt/interrupt.asm -o out/interruptasm.o
out/deal_int.o:int/interrupt/deal_int.c
	$(wlc) int/interrupt/deal_int.c -o out/deal_int.o
out/clk.o:int/clk/clk.c
	$(wlc) int/clk/clk.c -o out/clk.o
out/clkasm.o:int/clk/clk.asm
	$(wln) int/clk/clk.asm -o out/clkasm.o
out/do_clk.o:int/clk/do_clk.c
	$(wlc) int/clk/do_clk.c -o out/do_clk.o
out/processasm.o:mm/pro/process.asm
	$(wln) mm/pro/process.asm -o out/processasm.o
out/process.o:mm/pro/process.c
	$(wlc) mm/pro/process.c -o out/process.o
out/sys_in_out.o:lib/sys_in_out.asm
	$(wln) lib/sys_in_out.asm -o out/sys_in_out.o
out/monit.o:dev/char/monit/monit.c
	$(wlc) dev/char/monit/monit.c -o out/monit.o
out/string.o:lib/str/string.c
	$(wlc) lib/str/string.c -o out/string.o
out/console.o:dev/char/console.c
	$(wlc) dev/char/console.c -o out/console.o
out/tty.o:dev/char/tty.c
	$(wlc) dev/char/tty.c -o out/tty.o
out/keyboard.o:dev/char/keyboard/keyboard.c
	$(wlc) dev/char/keyboard/keyboard.c -o out/keyboard.o
out/keyboardasm.o:dev/char/keyboard/keyboard.asm
	$(wln) dev/char/keyboard/keyboard.asm -o out/keyboardasm.o