all: main

main: %: %.o my_putchar.o my_putint.o my_getchar.o my_getint.o
	gcc -o $@ $^ -nostartfiles -no-pie

%.o: %.asm
	nasm -f elf64 -g -o $@ $<

clean:
	rm -f main *.o