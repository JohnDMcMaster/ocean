#	@(#)Makefile 1.25 08/19/96 Delft University of Technology
#
# This Makefile requires the environment variable MACHINE to be set to the name
# of your hardware/software platform. For instance, in the csh type:
#
#         %  setenv MACHINE vax ; make install
#
# to compile the objects into objects/vax/*.o, make an archive in lib/vax/*.a
# and install the PROGRAM in the directory INSTALLDIR/bin/vax/*
#
# This Makefile is carefully constructed so that you can even run several
# "make"s concurrently in the same source directory, as long as the $MACHINE
# environments are different.


##############################################################################
#                                                                            #
#		      C U S T O M I Z A T I O N   A R E A                    #
#                                                                            #
##############################################################################

# name of the program:
PROGRAM = $(BINDIR)/libtest

# the name of the archive to contain the object files:
ARCHIVE = $(LIBDIR)/libocean.a

# the local header files:
LIBOC_HEADERS = \
        BinaryTree.h \
	RedBlackTree.h \
	sdfset.h \
	graph.h \
	sdfGraph.h \
	sdfNameIter.h \
	hashTable.h \
	priorQ.h \
	graphSteiner.h

CLASS_HEADERS = \
	Array.h \
	Char.h \
	ClsDesc.h \
	Container.h \
	Double.h \
	Int.h \
	Iterator.h \
	Long.h \
	Object.h \
	Point.h \
	Rectangle.h \
	Reference.h \
	Root.h \
	Sortable.h \
	String.h 

C_HEADERS = \
	regex.h

HEADERS = $(LIBOC_HEADERS) $(CLASS_HEADERS) $(C_HEADERS)

# objects for ANSI-C compilation. For each object $(OBJDIR)/*.o there
# must exist a corresponding *.c file in the current directory:
LIBOC_C_OBJECTS = \
	$(OBJDIR)/regex.o

# objects for C++ compilation. For each object $(OBJDIR)/*.o there
# must exist a corresponding *.C file in the current directory:
LIBOC_CXX_OBJECTS = \
	$(OBJDIR)/BinaryTree.o \
	$(OBJDIR)/RedBlackTree.o \
	$(OBJDIR)/sdfset.o \
	$(OBJDIR)/graph.o \
	$(OBJDIR)/sdfGraph.o \
	$(OBJDIR)/sdfNameIter.o \
	$(OBJDIR)/hashTable.o \
	$(OBJDIR)/priorQ.o \
	$(OBJDIR)/graphSteiner.o

CLASS_CXX_OBJECTS = \
	$(OBJDIR)/Array.o \
	$(OBJDIR)/Char.o \
	$(OBJDIR)/Container.o \
	$(OBJDIR)/Double.o \
	$(OBJDIR)/Int.o \
	$(OBJDIR)/Iterator.o \
	$(OBJDIR)/Long.o \
	$(OBJDIR)/Nil.o \
	$(OBJDIR)/Object.o \
	$(OBJDIR)/Point.o \
	$(OBJDIR)/Rectangle.o \
	$(OBJDIR)/Root.o \
	$(OBJDIR)/Sortable.o \
	$(OBJDIR)/String.o

# main is seperate because we don't want to include it in the archive:
MAIN = $(OBJDIR)/main.o

CXX_OBJECTS = $(CLASS_CXX_OBJECTS) $(LIBOC_CXX_OBJECTS)

# the root directory of the ocean tree:
OCEAN = /usr1/ocean

# directory where the ocean header files live:
SEAINCLDIR = $(OCEAN)/include

# directory where the ocean libraries live:
SEALIBDIR = $(OCEAN)/lib/${MACHINE}

# linker options to include ocean libraries:
SEALIBS = -locean -lseadif

# linker options to include all required libraries:
OLIBS = $(SEALIBS) $(XOLIBS)

# eXtra OLIBS:
XOLIBS = -ll

# overide at the command line (XCFLAGS=-g) if you want debugging information:
XCFLAGS= -O

# the ANSI-C compiler:
CC = cc -Aa -D_HPUX_SOURCE -D__STDC__=1

# options for the ANSI-C compiler:
CFLAGS = $(XCFLAGS) -I. -I$(SEAINCLDIR) -DREGEX_MALLOC

# the C++ compiler:
CXX = CC

# options for the C++ compiler:
CXXFLAGS = $(XCFLAGS) -I. -I$(SEAINCLDIR)

# the linker:
LD = CC

# options for the linker (does NOT include de -l options in OLIBS):
LDFLAGS = -L$(SEALIBDIR)

# the archive program:
AR = ar

# options for AR:
ARFLAGS = ruv

# how to build a symboltable for the archive:
RANLIB = ranlib

# directory to install the PROGRAM and the ARCHIVE:
INSTALLDIR = $(OCEAN)

# some targets (noticeably checkout_and_install) may call an inferior "make".
# This defines which macros will be inherited by the inferior make process:
INHERIT_MACROS = \
	"OCEAN=$(OCEAN)" \
	"INSTALLDIR=$(INSTALLDIR)" \
	"XLYCFLAGS=$(XLYCFLAGS)" \
	"XLDCFLAGS=$(XLDFLAGS)" \
	"XCFLAGS=$(XCFLAGS)"

##############################################################################
##                                                                          ##
##			     S T A T I C   A R E A                          ##
##                                                                          ##
##############################################################################

# the shell to exec the command lines in this Makefile. Should be /bin/sh:
SHELL = /bin/sh

# subdirectories to contain the objects, the archive and the executable:
OBJDIR = objects/$(MACHINE)
LIBDIR = lib/$(MACHINE)
BINDIR = bin/$(MACHINE)

# a scratch directory to check-out, build and install the latest version:
BUILDDIR = TMP.install

# a scratch dir to check-out the latest version and build a tar archive:
DISTRIBUTIONDIR = TMP.distrib

# All subdirectories that must exist before attempting compilation. In
# this list parent directories must appear before descendant directories:
SUBDIRS = objects $(OBJDIR) lib $(LIBDIR) bin $(BINDIR)

# All subdirectories of the INSTALLDIR that must exist before attempting
# installation. In this list parent directories must appear before descendant
# directories:
INSTALLDIRS = $(INSTALLDIR) \
	      $(INSTALLDIR)/bin \
	      $(INSTALLDIR)/$(BINDIR) \
	      $(INSTALLDIR)/lib \
	      $(INSTALLDIR)/$(LIBDIR)

# all the objects zusammen:
OBJECTS = $(LIBOC_C_OBJECTS) $(CXX_OBJECTS)

# file name suffixes recognized by this Makefile:
.SUFFIXES: .o .c .y .l .C

##############################################################################
##                                                                          ##
##			D E P E N D E N C Y   R U L E S                     ##
##                                                                          ##
##############################################################################

# the default rule. Build the archive if no target is specified:
default: archive

# build the program:
program: $(PROGRAM)
$(PROGRAM): typeoutProgram $(SUBDIRS) $(OBJECTS) $(MAIN)
	$(LD) $(LDFLAGS) \
	      $(MAIN) $(OBJECTS) \
	      $(OLIBS) \
	      -o $@

# build the archive:
archive: $(ARCHIVE)
$(ARCHIVE): typeoutArchive $(SUBDIRS) $(OBJECTS)
	$(AR) $(ARFLAGS) $(ARCHIVE) $(OBJECTS)
	$(RANLIB) $(ARCHIVE)

# install the program and the archive in the INSTALLDIR:
install: checkmachine $(ARCHIVE) $(INSTALLDIRS)
	rm -f $(INSTALLDIR)/$(ARCHIVE)
	cp $(ARCHIVE) $(INSTALLDIR)/$(ARCHIVE)
	@( \
	 set +e ; \
	 echo installing global header files if necessary ... ; \
	 for f in $(HEADERS); do \
	     if [ ! -f $(INSTALLDIR)/include/$$f ] ; then \
	        echo "\tcp $$f $(INSTALLDIR)/include/$$f"; \
	        rm -f $(INSTALLDIR)/include/$$f; \
	        cp $$f $(INSTALLDIR)/include/$$f; \
	     elif cmp -s $(INSTALLDIR)/include/$$f $$f; then \
	        true ; \
	     else \
		echo "\tcp $$f $(INSTALLDIR)/include/$$f"; \
	        rm -f $(INSTALLDIR)/include/$$f; \
		cp $$f $(INSTALLDIR)/include/$$f; \
	     fi ; \
	 done \
	)

# only compile the liboc sources:
liboc: $(LIBOC_CXX_OBJECTS) $(LIBOC_C_OBJECTS)

# only compile the classlib souces:
class: $(CLASS_CXX_OBJECTS)

# go to a scratch directory, check-out the latest SCCS version and install it:
checkout_and_install: checkmachine $(BUILDDIR)
	@( \
	SOURCEDIR=`pwd` ; \
	echo cd $(BUILDDIR) ; cd $(BUILDDIR) ; \
	echo make -f $$SOURCEDIR/Makefile veryclean ; \
	make -f $$SOURCEDIR/Makefile veryclean ; \
	echo sccs get -s $$SOURCEDIR/SCCS ; \
	sccs get -s $$SOURCEDIR/SCCS ; \
	echo make -f $$SOURCEDIR/Makefile $(INHERIT_MACROS) install ; \
	make -f $$SOURCEDIR/Makefile \
		$(INHERIT_MACROS) \
		install ; \
	)

# go to a scratch directory, check-out the latest SCCS version, and build
# a tar archive from it. Also uuencode the archive and prepare a shell script
# to mail the splitted uuencode archive:
distribution: checkmachine $(DISTRIBUTIONDIR)
	@( \
	echo building a distribution in $(DISTRIBUTIONDIR) ... ; \
	SOURCEDIR=`pwd` ; \
	echo cd $(DISTRIBUTIONDIR) ; \
	cd $(DISTRIBUTIONDIR) ; \
	SOURCENAME=`basename $$SOURCEDIR` ; \
	if [ ! -d $$SOURCENAME ] ; then mkdir $$SOURCENAME; fi ; \
	cd $$SOURCENAME ; \
	echo sccs get -s $$SOURCEDIR/SCCS ; \
	sccs get -s $$SOURCEDIR/SCCS ; \
	cd .. ; rm -f $$SOURCENAME.tar.Z ; \
	echo "tar cf - $$SOURCENAME | compress > $$SOURCENAME.tar.Z" ; \
	tar cf - $$SOURCENAME | compress > $$SOURCENAME.tar.Z ; \
	touch xaa ; rm -rf x?? $$SOURCENAME.uue $$SOURCENAME ; \
	echo uuencode $$SOURCENAME.tar.Z ; \
	uuencode $$SOURCENAME.tar.Z ; \
	echo split $$SOURCENAME.uue ; \
	split $$SOURCENAME.uue ; \
	SCRIPT=mail-$$SOURCENAME ; rm -f $$SCRIPT ; \
	echo creating shell script \"$$SCRIPT\" ; \
	echo "#!/bin/sh\n#\n# usage: $$SCRIPT recipient" > $$SCRIPT ; \
	echo "# Sends the current $$SOURCENAME release to recipient\n" \
	     >> $$SCRIPT ; \
	echo "for f in $$SOURCEDIR/$(DISTRIBUTIONDIR)/x??;" >> $$SCRIPT ; \
	echo "do\n   part=\`basename \$$f\`" >> $$SCRIPT ; \
	echo "   mailx -s \"$$SOURCENAME (part \$$part)\" \$$* < \$$f" \
	     >> $$SCRIPT ; \
	echo "   echo sending part \$$part to \$$*; sleep 10" >> $$SCRIPT ; \
	echo "done" >> $$SCRIPT ; \
	chmod +x $$SCRIPT ; \
	)

# if these directories do not already exist, create 'm:
$(SUBDIRS) $(INSTALLDIRS) $(BUILDDIR) $(DISTRIBUTIONDIR):; mkdir $@

# remove object files, the program and the archive:
clean: checkmachine
	rm -f $(OBJECTS) \
	      $(PROGRAM) \
	      $(ARCHIVE) core

# remove object files, lex and yacc output, the program and the archive:
veryclean: checkmachine
	rm -f $(OBJECTS) \
	      $(YACCOUTPUT) $(LEXOUTPUT) \
	      $(PROGRAM) \
	      $(ARCHIVE) core

# remove lex and yacc output files:
yyclean: checkmachine
	rm -f $(LYOBJECT) $(YACCOUTPUT) $(LEXOUTPUT)

libocclean: checkmachine
	rm -f $(LIBOC_CXX_OBJECTS)

classclean: checkmachine
	rm -f $(CLASS_CXX_OBJECTS)

# remove the program and the archive:
targetclean: checkmachine
	rm -f $(ARCHIVE) $(PROGRAM)

# remove the distribution scratch directory and its contents:
distclean:
	rm -rf $(DISTRIBUTIONDIR)

# clean up the directory used by the checkout_and_install rule:
buildclean: checkmachine
	-@( \
	SOURCEDIR=`pwd` ; \
	echo cd $(BUILDDIR) ; \
	cd $(BUILDDIR) ; \
	echo make -f $$SOURCEDIR/Makefile veryclean ; \
	make -f $$SOURCEDIR/Makefile veryclean ; \
	CXX_SOURCES=`echo $(CXX_OBJECTS) | sed -e 's;[^ ]*/;;g' -e 's/\.o/.C/g'` ; \
	echo "rm -f $$CXX_SOURCES [mM]akefile $(HEADERS)" ; \
	rm -f $$CXX_SOURCES [mM]akefile $(HEADERS) ; \
	)

# anounce the building of the program:
typeoutProgram: checkmachine
	@echo ''
	@echo "_______________ making $(PROGRAM) _______________"
	@echo ''

# anounce the building of the archive:
typeoutArchive: checkmachine
	@echo ''
	@echo "_______________ making $(ARCHIVE) _______________"
	@echo ''

# abort if the MACHINE environment variable is not set:
checkmachine:
	@if [ "$(MACHINE)" = "" ] ; then echo \
	    "\n\n" \
	    "******************************************************\n" \
	    "PLEASE SET YOUR \$$MACHINE ENVIRONMENT VARIABLE FIRST...\n"  \
	    "******************************************************\n" \
	    "For example, type this to your csh:\n\n" \
	    "     % setenv MACHINE hp800 ; make\n\n" ; \
	    exit 1 ; \
	 fi

# macros that expand to compilation commands for C++ sources:
DOCXX =	$(CXX) $(CXXFLAGS) -c $(@F:.o=.C) -o $@

# macros that expand to compilation commands for ANSI-C sources:
DOCC =	$(CC) $(CFLAGS) -c $(@F:.o=.c) -o $@

# the final dependency for all the liboc C++ sources:
$(LIBOC_CXX_OBJECTS): $(OBJDIR)/%.o: %.C $(LIBOC_HEADERS)
	$(DOCXX)

# the final dependency for all the classlib C++ sources:
$(CLASS_CXX_OBJECTS): $(OBJDIR)/%.o: %.C $(CLASS_HEADERS)
	$(DOCXX)

# the final dependency for all the ANSI-C sources:
$(LIBOC_C_OBJECTS): $(OBJDIR)/%.o: %.c $(C_HEADERS)
	$(DOCC)

# main depends on all headers:
$(MAIN): $(OBJDIR)/%.o: %.C $(HEADERS)
	$(DOCXX)

##############################################################################
##                                                                          ##
##			 E N D   O F   M A K E F I L E                      ##
##                                                                          ##
##############################################################################
