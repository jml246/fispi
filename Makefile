SRCDIR	=	$(SRCROOT)
OBJDIR	=	$(OBJROOT)
VPATH	=	$(SRCROOT)	

include MakeConfig

all: fisio_t fisalg_t fiscore_t

fisio_t:
	$(MAKE) -C ./fisio/

fiscom_t:
	$(MAKE) -C ./fiscom/

fisalg_t:
	$(MAKE) -C ./fisalg/

fiscore_t:
	$(MAKE) -C ./fiscore/

depend:
	$(FIND) . -maxdepth 1 -name "fis*" -type d -exec $(MAKE) -C {} localdepend \;

clean:
	$(FIND) . -maxdepth 1 -name "fis*" -type d -exec $(MAKE) -C {} localclean \;

install:
	$(FIND) . -maxdepth 1 -name "fis*" -type d -exec $(MAKE) -C {} localinstall \;

localinstall:
	$(MKDIR) -p $(INSTROOT)/

include MakeRules

-include MakeDep

