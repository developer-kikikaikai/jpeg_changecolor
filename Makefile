TARGET=convert2YCbCr422
SRC=main.c
CFLAG=-g -O0 

all:
	gcc ${CFLAG} ${SRC} -l jpeg -o ${TARGET}

clean:
	if [ -e ${TARGET} ]; then rm ${TARGET}; fi
