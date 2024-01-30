##############################################################################
##                              FREXXWARE
##############################################################################
#
# Project: Frexx C Preprocessor
#
##############################################################################

# Frexx PreProcessor Makefile

DEFINES     = -Dunix -Dpdc -DUNIX -DDEBUG
DEBUGFLAG	= -g
LD	        = ld
LDFLAGS		=
LIB         = libfpp.a
CPP         = fpp
FILECPP     = fcpp
TEMP		= templib.o
EXPORT		= fpp.exp
PREFIX      = /usr/local
CC          = gcc
CFLAGS		= $(DEBUGFLAG) $(DEFINES)
AR      	= ar
ARFLAGS 	= rv
.SUFFIXES: .o .c .c~ .h .h~ .a .i
OBJS 		= cpp1.o cpp2.o cpp3.o cpp4.o cpp5.o cpp6.o
FILEOBJS 		= cpp1.o cpp2.o cpp3.o cpp4.o cpp5.o cpp6.o usecpp.o

# ** compile cpp
#

#all: $(LIB) $(CPP)
all: $(FILECPP)

$(LIB) : $(OBJS) $(EXPORT)
	$(LD) $(OBJS) -o $(TEMP) -bE:$(EXPORT) -bM:SRE -T512 -H512 -lc
	rm -f $(LIB)
	$(AR) $(ARFLAGS) $(LIB) $(TEMP)
	rm $(TEMP)

$(CPP) : usecpp.c
	$(CC) $(CFLAGS) -o $(CPP) usecpp.c -L. -lfpp

$(FILECPP) : $(FILEOBJS)
	$(CC) $(FILEOBJS) -o $(FILECPP)

.c.o:
	$(CC) $(CFLAGS) -c $<

cpp1.o:cpp1.c cppdef.h
cpp2.o:cpp2.c cppdef.h
cpp3.o:cpp3.c cppdef.h
cpp4.o:cpp4.c cppdef.h
cpp5.o:cpp5.c cppdef.h
cpp6.o:cpp6.c cppdef.h
memory.o:memory.c

usecpp.o:usecpp.c

clean :
	rm -f *.o $(FILECPP)

tgz:
	rm -f makefile*~
	(dir=`pwd`;name=`basename $$dir`;echo Creates $$name.tar.gz; cd .. ; \
	tar -cf $$name.tar `ls $$name/*.[ch] $$name/*.exp $$name/*.fd $$name/makefile*` ; \
	gzip $$name.tar ; chmod a+r $$name.tar.gz ; mv $$name.tar.gz $$name/)

install: $(FILECPP)
	install "$(FILECPP)" "$(DESTDIR)$(PREFIX)/bin"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/bin/$(FILECPP)"
