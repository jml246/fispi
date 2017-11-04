SRCDIR  =      	.
OBJDIR  =       $(OBJROOT)/fisio/
VPATH   =       $(SRCDIR)

SRC     =      	$(SRCDIR)/fisio.c
OBJ     =      	fisio.o

include ../MakeConfig

all: fisio

fisio: $(OBJ) 
	$(CC) $(OBJ) -lwiringPi -lwiringPiDev -o $@ 

depend: localdepend

clean: localclean

install: localinstall

localinstall:
	$(MKDIR) -p $(INSTROOT)/lib/
	$(CP) -f fisio $(INSTROOT)/


include ../MakeRules
-include MakeDep
