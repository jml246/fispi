SRCDIR  =      	.
OBJDIR  =       $(OBJROOT)/fiscore/
VPATH   =       $(SRCDIR)

SRC     =      	$(SRCDIR)/fiscore.c
OBJ     =      	fiscore.o

include ../MakeConfig

all: fiscore 

fiscore: $(OBJ) 
	$(CC) $(OBJ) -o $@ 

depend: localdepend

clean: localclean

install: localinstall

localinstall:
	$(MKDIR) -p $(INSTROOT)/
	$(CP) -f fiscore $(INSTROOT)/


include ../MakeRules
-include MakeDep
