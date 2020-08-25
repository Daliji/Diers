
bootfiles = tools/boot.bin tools/setup.bin tools/jump.bin tools/kernel

Diers.img:tools/Diers.img
	cp tools/Diers.img ./Diers.img
	mv in/out/* tools
tools/Diers.img:$(bootfiles)
	tools/write < tools/in1.txt
	tools/write < tools/in2.txt
	tools/write < tools/in3.txt
	tools/write < tools/in4.txt

tools/boot.bin:in/boot/boot.asm
	nasm in/boot/boot.asm -o tools/boot.bin
tools/setup.bin:in/boot/setup.asm
	nasm in/boot/setup.asm -o tools/setup.bin
tools/jump.bin:in/boot/jump.asm
	nasm -o tools/jump.bin in/boot/jump.asm
tools/kernel:in/out/kernel
	mv in/out/kernel tools
in/out/kernel:
	(cd in; make)

clean:
	rm -f tools/*.bin tools/kernel tools/*.o
	rm -f in/out/*