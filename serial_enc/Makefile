#
# Makefile by MED on 07-03-2025
#

#----------------------------------------
# Set the project name (executable file name)
TARGET = serial_enc_test

#----------------------------------------
# Set the compiler
CC = gcc    #for use the GNU C Compiler
# CC = clang    #for the LLVM Project Compiler

#----------------------------------------
# Set the compiler Flags
# CFLAG = -g    #for DEBUG info
# CFLAG = -O2    #for RELEASE optimize O1 .. O2 .. Os
# CFLAG = -O0    #for RELEASE no optimize

#----------------------------------------
# Set the linker Flags
# LDFLAGS = 

#----------------------------------------
# Set the libraries used
# LIBS = -lm    #the math lib (libm.a) in /usr/lib or /usr/lib64
# LIBS = -lncurses    #the curses lib

#----------------------------------------
# List here the source files
SRC = serial_enc_test.c
SRC += crc_enc.c
# SRC += libcsv.c


OBJS = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@
	gtags -q

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

# test_ncurses:
# 	$(CC) test_ncurses.c $(LIBS)

clean:
	rm -f *.o
	rm -f *.out
	rm -f $(TARGET)
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov

# *** EOF ***
