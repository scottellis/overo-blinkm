# Makefile for compiling on the overo 
#

CC = gcc

CFLAGS = -g -Wall -Wextra -Werror
		   
TARGET = blinkm

OBJS = main.o \
       utility.o \
       i2c_functions.o \
       i2c_blinkm.o 


${TARGET} : $(OBJS)
	${CC} ${CFLAGS} ${OBJS} -o ${TARGET}


main.o: main.c 
	${CC} ${CFLAGS} -c main.c 

utility.o: utility.c 
	${CC} ${CFLAGS} -c utility.c

i2c_functions.o: i2c_functions.c 
	${CC} ${CFLAGS} -c i2c_functions.c 

i2c_blinkm.o: i2c_blinkm.c blinkm_regs.h 
	${CC} ${CFLAGS} -c i2c_blinkm.c


clean:
	rm -f ${TARGET} ${OBJS} *~


