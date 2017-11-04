SRCDIR  =      	.
OBJDIR  =       $(OBJROOT)/fisled/
VPATH   =       $(SRCDIR)

SRC     =      	$(SRCDIR)/fisled.c
OBJ     =      	fisled.o

include ../MakeConfig

all: fisled

fisled: $(OBJ) 
	$(CC) $(OBJ) -o $@ 

depend: localdepend

clean: localclean

install: localinstall

localinstall:
	$(MKDIR) -p $(INSTROOT)/lib/
	$(CP) -f fisled $(INSTROOT)/


include ../MakeRules
-include MakeDep
