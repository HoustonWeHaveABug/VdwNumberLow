VDW_NUMBER_LOW_C_FLAGS=-O2 -std=c99 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wstrict-prototypes -Wswitch -Wwrite-strings

vdw_number_low: vdw_number_low.o
	gcc -o vdw_number_low vdw_number_low.o

vdw_number_low.o: vdw_number_low.c vdw_number_low.make
	gcc -c ${VDW_NUMBER_LOW_C_FLAGS} -o vdw_number_low.o vdw_number_low.c

clean:
	rm -f vdw_number_low vdw_number_low.o
