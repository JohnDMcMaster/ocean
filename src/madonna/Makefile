#	@(#)Makefile 1.22 09/01/99 Delft University of Technology
#
# This Makefile requires the environment variable MACHINE to be set to the name
# of your hardware/software platform. For instance, in the csh type:
#
#         %  setenv MACHINE vax ; gnumake install
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
PROGRAM = $(BINDIR)/madonna

# directory to install the PROGRAM
INSTALLDIR = $(OCEAN)

# the root directory of the ocean tree:
OCEAN = /usr1/ocean

# directory where the ocean libraries live:
SEALIBDIR = $(OCEAN)/lib/${MACHINE}

# linker options to include ocean libraries:
SEALIBS = -locean -lseadif

# eXtra OLIBS:
XOLIBS = -lm

# linker options to include all required libraries:
OLIBS = -lmain -lpartitioner -lphil -llia $(SEALIBS) $(XOLIBS)

# the ANSI-C compiler:
CC = CC

# the C++ compiler:
CXX = CC

# overide at the command line (XCFLAGS=-g) if you want debugging information:
XCFLAGS= -O

# the linker:
LD = CC

# eXtra flags for the linker:
XLDFLAGS = $(XCFLAGS)

# options for the linker (does NOT include de -l options in OLIBS):
LDFLAGS = $(XLDFLAGS) -L$(LIBDIR) -L$(SEALIBDIR)

# the SUBSOURCES are subdirectories that contain sources and a Makefile:
SUBSOURCES = main lia phil partitioner

# the lexical analyzer generator:
LEX = flex

# options for LEX:
LEXFLAGS = -F

# the parser generator:
YACC = yacc++

# options for YACC.  Set this to -l if you want to use the symbolic debugger
# with yacc generated C-code. (In this case, do not forget to add -g to the
# XLYCFLAGS.) Remove the -l option if you still have syntax errors in the yacc
# source:
YACCFLAGS = 

# the archive program:
AR = ar

# options for AR:
ARFLAGS = ruv

# how to build a symboltable for the archive:
RANLIB = ranlib

# some targets (noticeably checkout_and_install) may call an inferior "gnumake".
# This defines which macros will be inherited by the inferior make process.
# Use the construct $${SOURCEDIR} to refer to the current source directory:
INHERIT_MACROS = \
	"OCEAN=$(OCEAN)" \
	"LIBDIR=$${SOURCEDIR}$(LIBDIR)" \
	"CXX=$(CXX)" \
	"CC=$(CC)" \
	"XCFLAGS=$(XCFLAGS)" \
	"LEX=$(LEX)" \
	"LEXFLAGS=$(LEXFLAGS)" \
	"YACC=$(YACC)" \
	"YACCFLAGS=$(YACCFLAGS)"

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
SUBDIRS = lib $(LIBDIR) bin $(BINDIR)

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
$(PROGRAM): typeoutProgram $(SUBDIRS) $(SUBSOURCES)
	$(LD) $(LDFLAGS) \
	      $(OLIBS) \
	      -o $@

$(SUBSOURCES): checkmachine $(SUBDIRS)
	@( \
	SOURCEDIR=`pwd` ; \
	echo cd $@; \
	cd $@; \
	echo $(MAKE) ARCHIVE=$$SOURCEDIR/$(LIBDIR)/lib$(@).a ; \
	$(MAKE) ARCHIVE=$$SOURCEDIR/$(LIBDIR)/lib$(@).a || exit $$?; \
	echo "\n" ; \
	)

# install the program in the INSTALLDIR:
install: checkmachine $(PROGRAM) $(INSTALLDIRS)
	@( \
	  if [ -f $(INSTALLDIR)/$(PROGRAM) ] ; then \
	     echo mv -f $(INSTALLDIR)/$(PROGRAM) $(INSTALLDIR)/$(PROGRAM).old; \
	     mv -f $(INSTALLDIR)/$(PROGRAM) $(INSTALLDIR)/$(PROGRAM).old; \
	  fi ; \
	  echo cp $(PROGRAM) $(INSTALLDIR)/$(PROGRAM) ; \
	  cp $(PROGRAM) $(INSTALLDIR)/$(PROGRAM) ; \
	)

# go to a scratch directory, check-out the latest SCCS version and install it:
checkout_and_install: checkmachine $(BUILDDIR)
	@( \
	SOURCEDIR=`pwd`/ ; \
	echo cd $(BUILDDIR) ; cd $(BUILDDIR) ; \
	echo $(MAKE) -f $${SOURCEDIR}Makefile veryclean ; \
	$(MAKE) -f $${SOURCEDIR}Makefile veryclean ; \
	echo sccs get -s $${SOURCEDIR}SCCS ; \
	sccs get -s $${SOURCEDIR}SCCS ; \
	echo $(MAKE) -f $${SOURCEDIR}Makefile $(INHERIT_MACROS) install ; \
	$(MAKE) -f $${SOURCEDIR}Makefile \
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
clean veryclean: checkmachine
	@( \
	 echo rm -f $(PROGRAM); rm -f $(PROGRAM);  \
	 SOURCEDIR=`pwd`/ ; \
	 for SUBSOURCE in $(SUBSOURCES); do \
	     echo "\n\n(cd $$SUBSOURCE;\n $(MAKE) $@ $(INHERIT_MACROS))\n" ; \
	     (cd $$SUBSOURCE; $(MAKE) $@ $(INHERIT_MACROS)) ; \
	 done ; \
	 )

# remove the program and the archive:
targetclean: checkmachine
	rm -f $(ARCHIVE) $(PROGRAM)

# remove the distribution scratch directory and its contents:
distclean:
	rm -rf $(DISTRIBUTIONDIR)

# clean up the directory used by the checkout_and_install rule:
buildclean: checkmachine
	-@( \
	SOURCEDIR=`pwd`/ ; \
	echo cd $(BUILDDIR) ; \
	cd $(BUILDDIR) ; \
	echo $(MAKE) -f $${SOURCEDIR}Makefile veryclean ; \
	$(MAKE) -f $${SOURCEDIR}Makefile veryclean ; \
	C_SOURCES=`echo $(C_OBJECTS) | sed -e 's;[^ ]*/;;g' -e 's/\.o/.c/g'` ; \
	echo "rm -f $$C_SOURCES [mM]akefile $(HEADERS)" ; \
	rm -f $$C_SOURCES [mM]akefile $(HEADERS) ; \
	)

# anounce the building of the program:
typeoutProgram: checkmachine
	@echo ''
	@echo "_______________ making $(PROGRAM) _______________"
	@echo ''

# abort if the MACHINE environment variable is not set:
checkmachine:
	@if [ "$(MACHINE)" = "" ] ; then echo \
	    "\n\n" \
	    "******************************************************\n" \
	    "PLEASE SET YOUR \$$MACHINE ENVIRONMENT VARIABLE FIRST...\n"  \
	    "******************************************************\n" \
	    "For example, type this to your csh:\n\n" \
	    "     % setenv MACHINE hp800 ; $(MAKE)\n\n" ; \
	    exit 1 ; \
	 fi

##############################################################################
##                                                                          ##
##			 E N D   O F   M A K E F I L E                      ##
##                                                                          ##
##############################################################################
