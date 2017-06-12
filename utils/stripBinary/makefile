filename = stripBinary

CFLAGS= -Wall
CC= g++
GC= gcc

EXEDIR=C:/bin
APPOBJ=C:/appobj
OBJDR=C:/obj
GENCPPLIB=../gclib
OBJL = $(APPOBJ)/$(filename).o

$(EXEDIR)/$(filename).exe : $(APPOBJ)/$(filename).o
	$(CC) $(CFLAGS) $(OBJL) -I$(GENCPPLIB) -o $@

$(APPOBJ)/$(filename).o : $(filename).cpp $(GENCPPLIB)/CommandLine.h
	$(CC) $(CFLAGS) -I$(GENCPPLIB) -c $< -o $@




