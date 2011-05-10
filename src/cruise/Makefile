# SccsId = @(#)Makefile 1.10  10/31/94
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
PROGRAM = $(BINDIR)/cruise

HEADERS = SeeDif.h \
    SeeDif-core.h \
    DeleteDialog-core.h \
    DeleteDialog.h \
    MadonnaDialog-core.h \
    MadonnaDialog.h \
    TroutDialog-core.h \
    TroutDialog.h

# main is seperate because we don't want to include it in the archive:
MAIN = $(OBJDIR)/cruise-main.o

OBJECTS = $(OBJDIR)/SeeDif.o \
    $(OBJDIR)/SeeDif-core.o \
    $(OBJDIR)/DeleteDialog-core.o \
    $(OBJDIR)/DeleteDialog.o \
    $(OBJDIR)/MadonnaDialog-core.o \
    $(OBJDIR)/MadonnaDialog.o \
    $(OBJDIR)/TroutDialog-core.o \
    $(OBJDIR)/TroutDialog.o \
    $(OBJDIR)/runprog.o

# the root directory of the ocean tree:
OCEAN = /usr1/ocean

# directory where the ocean header files live:
OCEANINCLDIR = $(OCEAN)/include


# directories where the ocean libraries live:
OCEANLIBDIR = $(OCEAN)/lib/$(MACHINE)

# the directory where the InterViews package lives:
IV_INSTALLED = /usr/local/interviews

# the directory where the InterViews libraries live:
IVLIBDIR = $(IV_INSTALLED)/lib/$(CPU)

# the directories where the InterViews include files live:

IVINCLDIR1 = $(IV_INSTALLED)/include/IV-2_6
IVINCLDIR2 = $(IV_INSTALLED)/include

# directory where the ocean manual pages live:
SEAMANDIR = $(OCEAN)/man

# linker options to include all required libraries:

IVLIBS = -lUnidraw -lIV -lXext -lX11 
OLIBS  = -locean  -lseadif
XOLIBS = -lm -ll 

# overide at the command line (XCFLAGS=-g) if you want debugging information:
XCFLAGS= -O

# the ANSI-C compiler:
CC = CC

# the C++ compiler:
CXX = CC

CXXSPECFLAGS = -Dcplusplus_2_0
# options for the C++ compiler:
IV_FLAGS = $(CXXSPECFLAGS) \
	   -Div2_6_compatible \
	   -DSYSV \
	   -I$(IVINCLDIR1) \
	   -I$(IVINCLDIR2)

CXXFLAGS =  $(XCFLAGS) $(IV_FLAGS) -I$(OCEANINCLDIR) 


# options for the ANSI-C compiler:
CFLAGS = $(CXXFLAGS)

BIGCC = $(CXX)

# the linker:
LD = $(CXX)

# eXtra flags for the linker:
XLDFLAGS = $(XCFLAGS)

# options for the linker (does NOT include de -l options in OLIBS):
LDFLAGS = $(XLDFLAGS) \
	  -L$(OCEANLIBDIR) \
	  -L$(IVLIBDIR)

# the archive program:
AR = ar

# options for AR:
ARFLAGS = ruv

# how to build a symboltable for the archive:
RANLIB = ranlib

# directory to install the PROGRAM and the ARCHIVE:
INSTALLDIR = $(OCEAN)

# the manual pages that document this thing:
MANPAGES = \
	cruise.1

# some targets (noticeably checkout_and_install) may call an inferior "make".
# This defines which macros will be inherited by the inferior make process:
INHERIT_MACROS = \
	"INSTALLDIR=$(INSTALLDIR)" \
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
MANDIR = man

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
	      $(INSTALLDIR)/$(LIBDIR) \
	      $(INSTALLDIR)/$(MANDIR) \
	      $(INSTALLDIR)/$(MANDIR)/man3 \
	      $(INSTALLDIR)/$(MANDIR)/man4

# all the objects zusammen:


# file name suffixes recognized by this Makefile:
.SUFFIXES: .o .c .y .l .C

##############################################################################
##                                                                          ##
##			D E P E N D E N C Y   R U L E S                     ##
##                                                                          ##
##############################################################################

# the default rule. Build the archive if no target is specified:
default: program

# build the program:
program: $(PROGRAM)
$(PROGRAM): typeoutProgram $(SUBDIRS) $(OBJECTS) $(MAIN) $(XOLIBS)
	$(LD) $(LDFLAGS) \
	      $(MAIN) $(OBJECTS) \
	      $(OLIBS) $(IVLIBS) $(XOLIBS) \
	      -o $@

# install the program and the archive in the INSTALLDIR:
install: checkmachine $(PROGRAM) $(MANPAGES) $(INSTALLDIRS)
	rm -f $(INSTALLDIR)/$(PROGRAM)
	cp $(PROGRAM) $(INSTALLDIR)/$(PROGRAM)
	@( \
	 echo installing manual pages if necessary ... ; \
	 for manpage in $(MANPAGES); do \
	     subman=man`echo $$manpage | sed 's/.*\(.\)/\1/'` ; \
	     if [ ! -f $(SEAMANDIR)/$$subman/$$manpage ] ; then \
	        echo "\tcp `basename $$manpage` $(SEAMANDIR)/$$subman/$$manpage" ; \
	        rm -f $(SEAMANDIR)/$$subman/$$manpage ; \
	        cp `basename $$manpage` $(SEAMANDIR)/$$subman/$$manpage ; \
	     else \
	     MP=`basename $$manpage` ; \
	     if cmp -s $$MP $(SEAMANDIR)/$$subman/$$manpage ; then \
	        true ; \
	     else \
	        echo "\tcp `basename $$manpage` $(SEAMANDIR)/$$subman/$$manpage" ; \
	        rm -f $(SEAMANDIR)/$$subman/$$manpage ; \
	        cp `basename $$manpage` $(SEAMANDIR)/$$subman/$$manpage ; \
	     fi; fi; \
	 done \
	)

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
	@(\
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
	rm -f $(OBJECTS) $(MAIN) \
	      $(PROGRAM) \
	      $(ARCHIVE) core

# remove object files, lex and yacc output, the program and the archive:
veryclean: checkmachine
	rm -f $(OBJECTS) $(MAIN) \
	      $(PROGRAM) \
	      $(ARCHIVE) core


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
	C_SOURCES=`echo $(OBJECTS) | sed -e 's;[^ ]*/;;g' -e 's/\.o/.c/g'` ; \
	echo "rm -f $$C_SOURCES [mM]akefile $(HEADERS)" ; \
	rm -f $$C_SOURCES [mM]akefile $(HEADERS) ; \
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

# abort if the MACHINE or CPU environment variable is not set:
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
	@if [ "$(CPU)" = "" ] ; then echo \
	    "\n\n" \
	    "******************************************************\n" \
	    "PLEASE SET YOUR \$$CPU ENVIRONMENT VARIABLE FIRST...\n"  \
	    "******************************************************\n" \
	    "It must contain the same string as when installing Interviews. \n\n" \
	    "For example, type this to your csh:\n\n" \
	    "     % setenv CPU HP800 ; make\n\n" ; \
	    exit 1 ; \
	 fi



# macros that expand to compilation commands for C++ sources:
DOCXX =	$(CXX) $(CXXFLAGS) -c $(@F:.o=.c) -o $@

$(MAIN): $(OBJDIR)/%.o: %.c $(HEADERS)
	$(DOCXX)

$(OBJECTS): $(OBJDIR)/%.o: %.c $(HEADERS)
	$(DOCXX)

##############################################################################
##                                                                          ##
##			 E N D   O F   M A K E F I L E                      ##
##                                                                          ##
##############################################################################

