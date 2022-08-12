# Author: Benjamin G. Friedman
# Date: 05/20/2022
# File: Makefile
# Description: Makefile for the matrix calculations program.


CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic #-Og -g -fsanitize=undefined
LDLIBS = -lm
EXE1 = MatrixCalculations
OBJ1 = Main.o Matrix.o Menu.o
EXES = $(EXE1)


all: $(EXES)
.PHONY: all clean


$(EXE1): $(OBJ1)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm $(EXES) $(wildcard *.o)
