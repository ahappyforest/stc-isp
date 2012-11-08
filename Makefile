INCLUDE = 
SOURCE  = protocol.c main.c uart.c
OBJ     = stcisp

all:
	gcc $(SOURCE) -o $(OBJ)

clean:
	rm -rf *.[aoS]
	rm -rf a.out
	rm -rf stcisp
	
