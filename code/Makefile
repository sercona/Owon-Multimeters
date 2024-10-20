LOCATION=/usr/local
CFLAGS=-O2 -Wall -g
LIBS=

OBJ=owon_multi_cli decode_only
default: owon_multi_cli

.c.o:
	${CC} ${CFLAGS} -c $*.c

all: ${OBJ} 

owon_multi_cli: ${OFILES} owon_multi_cli.c 
	${CC} ${CFLAGS} owon_multi_cli.c -o owon_multi_cli ${LIBS}

decode_only: ${OFILES} decode_only.c 
	${CC} ${CFLAGS} decode_only.c -o decode_only ${LIBS}

install: ${OBJ}
	cp owon_multi_cli ${LOCATION}/bin/

clean:
	rm -f *.o *core ${OBJ}
