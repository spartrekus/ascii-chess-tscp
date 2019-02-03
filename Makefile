OBJ = board.o data.o eval.o main.o search.o parse.o
CFLAGS = -g -DFICS=1 -O

tscp: $(OBJ)
	$(CC) -o tscp $(OBJ)

clean:
	/bin/rm -f $(OBJ) tscp *~


