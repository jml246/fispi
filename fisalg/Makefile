SRCDIR  =      	.
OBJDIR  =       $(OBJROOT)/fisalg/
VPATH   =       $(SRCDIR)

SRC     =      	$(SRCDIR)/fisalg.c \
		$(SRCDIR)/fis/fis_functions.c \
		$(SRCDIR)/fis/fis_post_proc.c \
		$(SRCDIR)/fis/fis.c
OBJ     =      	fisalg.o \
		fis/fis_functions.o \
		fis/fis_post_proc.o \
		fis/fis.o

include ../MakeConfig

all: fisalg

fisalg: $(OBJ) 
	$(CC) $(OBJ) -o $@ 

depend: localdepend

clean: localclean

install: localinstall

localinstall:
	$(MKDIR) -p $(INSTROOT)/lib/
	$(CP) -f fisalg $(INSTROOT)/


include ../MakeRules
-include MakeDep
