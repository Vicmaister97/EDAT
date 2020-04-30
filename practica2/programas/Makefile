CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LDLIBS = -lodbc

EXE = usuario oferta compra

all : $(EXE)

clean :
	rm -f *.o core $(EXE)

$(EXE) : % : %.o odbc.o
