LOCATION=/usr/local
CFLAGS=-O2 -Wall -g
LIBS=

OBJ=owon_multi_cli
default: owon_multi_cli

.c.o:
	${CC} ${CFLAGS} -c $*.c

all: ${OBJ} 

owon_multi_cli: ${OFILES} owon_multi_cli.c 
	${CC} ${CFLAGS} owon_multi_cli.c -o owon_multi_cli ${LIBS}

install: ${OBJ}
	cp owon_multi_cli ${LOCATION}/bin/

clean:
	rm -f *.o *core ${OBJ}
