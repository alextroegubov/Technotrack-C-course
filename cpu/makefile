all: cpu compiler stack.o disasm

cpu: cpu.c cpu.h stack.o const.h commands.h
	gcc -o cpu cpu.c stack.o -lm

compiler: compiler.c compiler.h const.h commands.h
	gcc -o compiler compiler.c 

stack.o: stack.c
	gcc -c -o stack.o stack.c

disasm: disasm.c commands.h
	gcc -o disasm disasm.c

clean:
	rm *.log
