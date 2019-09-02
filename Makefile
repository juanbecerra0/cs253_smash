#------------------------------------------------------------------------------
#
# Makefile --- Re-usable makefile for building many simple projects
#
# Usage
# make all Build the released product
# make clean Remove build artifacts
#
# Author(s)
# 01/11/18 Epoch............................................................jrc
#
#------------------------------------------------------------------------------
#-------Define names of all the object files in this project
OBJS = smash.o commands.o history.o
#-------Define the name of the resulting released product
EXE = smash
#-------Define options passed by make to the compiler and which compiler to use
CFLAGS = -Wall -std=c99
CC = gcc
#-------Define target "all" for building the executable(s)
all: rules.d $(EXE)
#-------Rebuild rules.d if it's out-of-date with any *.c or *.h file using gcc -MM
rules.d: $(wildcard *.c) $(wildcard *.h)
	gcc -MM $(wildcard *.c) >rules.d
#-------Incorporate the auto-generated dependencies from rules.d into this Makefile
-include rules.d
#-------Rule for linking the executable product
$(EXE): $(OBJS) smashLib.a
	$(CC) $(CFLAGS) $^ -o $@
clean:
	rm -f $(OBJS) $(EXE) *.d *.a

#-------Construct library
smashLib.a: $(OBJS)
	ar rcs smashLib.a $(OBJS)

libs: smashLib.a

#-------Build a DEBUG version
debug: CFLAGS += --DDEBUG -g -Og
debug: smash

#-------Targets builds a debug version and executes the product under valgrind
valgrind: debug
	valgrind --leak-check=yes --leak-check=full --show-leak-kinds=all smash
