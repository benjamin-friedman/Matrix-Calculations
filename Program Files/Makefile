CC = gcc
CFLAGS = -g -Wall --std=c99
OBJ = Main.o Matrix.o Menu.o
EXE = MatrixCalculations

$(EXE): $(OBJ)
	$(CC) $(CLFAGS) -o $(EXE) $(OBJ) -lm
Main.o: Main.c
	$(CC) $(CFLAGS) -c Main.c -o Main.o
Matrix.o: Matrix.c
	$(CC) $(CFLAGS) -c Matrix.c -o Matrix.o
Menu.o: Menu.c
	$(CC) $(CFLAGS) -c Menu.c -o Menu.o

clean:
	rm -rf $(EXE) $(OBJ)
