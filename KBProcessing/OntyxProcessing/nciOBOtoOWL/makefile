include ../lib/makefile.head
filename = nciOBOtoOWL

OBJL = $(APPOBJ)/$(filename).o $(OBJ)

$(EXEDIR)/$(filename).exe : $(APPOBJ)/$(filename).o $(OBJ)
	$(CC) $(CFLAGS) -Lc:/usr/lib $(OBJL) -I$(MYSRCLIB) -I$(GENCPPLIB) -lxerces-c -o $@

$(APPOBJ)/$(filename).o : $(filename).cpp \
		$(OBJDR)/fullSyn.o $(OBJDR)/OntyxKb.o
	$(CC) $(CFLAGS) -I$(MYSRCLIB) -I$(GENCPPLIB) -Ic:/usr/include -c $< -o $@


include ../lib/makefile.incl