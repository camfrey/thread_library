# Makefile for UD CISC user-level thread library

CC = gcc
CFLAGS = -g

LIBOBJS = t_lib.o 

TSTOBJS = test00.o test01.o test01x.o test01-shone.o test01-sullivan.o test03.o test10.o 3test.o rendezvous.o order.o philosophers.o

# specify the executable 

EXECS = test00 test01 test01x test01-shone test01-sullivan test03 test10 test03-shone rendezvous order philosophers

# specify the source files

LIBSRCS = t_lib.c

TSTSRCS = test00.c test01.c test01x.c test01-shone.c test01-sullivan.c test03.c test10.c 3test.c philosophers.c rendezvous.c order.c

# ar creates the static thread library

t_lib.a: ${LIBOBJS} Makefile
	ar rcs t_lib.a ${LIBOBJS}

# here, we specify how each file should be compiled, what
# files they depend on, etc.

t_lib.o: t_lib.c t_lib.h Makefile
	${CC} ${CFLAGS} -c t_lib.c

test00.o: test00.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c test00.c

test00: test00.o t_lib.a Makefile
	${CC} ${CFLAGS} test00.o t_lib.a -o test00

test01.o: test01.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c test01.c

test01: test01.o t_lib.a Makefile
	${CC} ${CFLAGS} test01.o t_lib.a -o test01

test01x.o: test01x.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c test01x.c

test01x: test01x.o t_lib.a Makefile
	${CC} ${CFLAGS} test01x.o t_lib.a -o test01x

test01-shone.o: test01-shone.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c test01-shone.c

test01-shone: test01-shone.o t_lib.a Makefile
	${CC} ${CFLAGS} test01-shone.o t_lib.a -o test01-shone

test01-sullivan.o: test01-sullivan.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c test01-sullivan.c

test01-sullivan: test01-sullivan.o t_lib.a Makefile
	${CC} ${CFLAGS} -pthread test01-sullivan.o t_lib.a -o test01-sullivan

test03.o: test03.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c test03.c

test03: test03.o t_lib.a Makefile
	${CC} ${CFLAGS} test03.o t_lib.a -o test03

test10.o: test10.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c test10.c

test10: test10.o t_lib.a Makefile
	${CC} ${CFLAGS} test10.o t_lib.a -o test10

3test.o: 3test.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c 3test.c

test03-shone: 3test.o t_lib.a Makefile
	${CC} ${CFLAGS} 3test.o t_lib.a -o test03-shone

philosophers.o: philosophers.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c philosophers.c

philosophers: philosophers.o t_lib.a Makefile
	${CC} ${CFLAGS} philosophers.o t_lib.a -o philosophers

rendezvous.o: rendezvous.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c rendezvous.c

rendezvous: rendezvous.o t_lib.a Makefile
	${CC} ${CFLAGS} rendezvous.o t_lib.a -o rendezvous

order.o: order.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c order.c

order: order.o t_lib.a Makefile
	${CC} ${CFLAGS} order.o t_lib.a -o order

clean:
	rm -f t_lib.a ${EXECS} ${LIBOBJS} ${TSTOBJS} 
