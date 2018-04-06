TARGET=convert2YCbCr422
SRC=$(shell find . -maxdepth 1 -name *.c)
CFLAG=-g -O2 
LIBS=-l jpeg
OBJ=$(SRC:%.c=%.o)

all: $(OBJ)
	gcc $(CFLAG) $(OBJ) $(LIBS) -o $(TARGET)

%.o: %.c
	gcc -o $@ -c $<

clean:
	rm -f $(OBJ) $(TARGET);
