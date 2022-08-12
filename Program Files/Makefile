# Author: Benjamin G. Friedman
# Date: 05/20/2021
# File: Makefile
# Description: Makefile for the matrix calculations program.

CC = gcc
CFLAGS = -g -Wall -std=c99
LDLIBS = -lm
OBJ = Main.o Matrix.o Menu.o
EXE = MatrixCalculations

$(EXE): $(OBJ)
	$(CC) $(CLFAGS) -o $(EXE) $(OBJ) $(LDLIBS)
Main.o: Main.c Menu.h
	$(CC) $(CFLAGS) -c Main.c -o Main.o
Matrix.o: Matrix.c Matrix.h
	$(CC) $(CFLAGS) -c Matrix.c -o Matrix.o
Menu.o: Menu.c Menu.h Matrix.h
	$(CC) $(CFLAGS) -c Menu.c -o Menu.o

clean:
	-rm $(EXE) $(OBJ)
