#	@(#)Makefile 1.13 (Delft University of Technology) 10/31/94
#
# This Makefile requires the environment variable MACHINE to be set to the name
# of your hardware/software platform. For instance, in the csh type:
#
#         %  setenv MACHINE vax ; make install
#
# to compile the objects into objects/vax/*.o and install the PROGRAM in the
# directory INSTALLDIR/bin/vax/*
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
PROGRAM = $(BINDIR)/makebus

# other names for this executable:
OTHERPROGRAMNAMES = $(BINDIR)/showbus

# the root directory of the ocean tree:
OCEAN = /usr1/ocean

# the local header files:
HEADERS = \
	globals.h \
	prototypes.h \
	seadifGraph.h

# objects for C++ compilation. For each object $(OBJDIR)/*.o there
# must exist a corresponding *.C file in the current directory:
CXX_OBJECTS = \
	$(OBJDIR)/classifyNet.o \
	$(OBJDIR)/globals.o \
	$(OBJDIR)/makebus.o \
	$(OBJDIR)/showbus.o \
	$(OBJDIR)/seadifGraph.o \
	$(OBJDIR)/main.o

# object that contains the date of compilation:
THEDATE = $(OBJDIR)/thedate.o

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

# the C++ compiler:
CXX = CC

# options for the C++ compiler:
CXXFLAGS = $(XCFLAGS) -I$(SEAINCLDIR)

# the linker:
LD = CC

# eXtra flags for the linker:
XLDFLAGS = $(XCFLAGS)

# options for the linker (does NOT include de -l options in OLIBS):
LDFLAGS = -L$(SEALIBDIR) $(XLDFLAGS)

# directory to install the PROGRAM:
INSTALLDIR = $(OCEAN)

# some targets (noticeably checkout_and_install) may call an inferior "make".
# This defines which macros will be inherited by the inferior make process:
INHERIT_MACROS = \
	"OCEAN=$(OCEAN)" \
	"INSTALLDIR=$(INSTALLDIR)" \
	"XCFLAGS=$(XCFLAGS)"

##############################################################################
##                                                                          ##
##			     S T A T I C   A R E A                          ##
##                                                                          ##
##############################################################################

# the shell to exec the command lines in this Makefile. Should be /bin/sh:
SHELL = /bin/sh

# subdirectories to contain the objects and the executable:
OBJDIR = objects/$(MACHINE)
BINDIR = bin/$(MACHINE)

# a scratch directory to check-out, build and install the latest version:
BUILDDIR = TMP.install

# a scratch dir to check-out the latest version and build a tar archive:
DISTRIBUTIONDIR = TMP.distrib

# All subdirectories that must exist before attempting compilation. In
# this list parent directories must appear before descendant directories:
SUBDIRS = objects $(OBJDIR) bin $(BINDIR)

# All subdirectories of the INSTALLDIR that must exist before attempting
# installation. In this list parent directories must appear before descendant
# directories:
INSTALLDIRS = $(INSTALLDIR) \
	      $(INSTALLDIR)/bin \
	      $(INSTALLDIR)/$(BINDIR)

# all the objects zusammen:
OBJECTS = $(CXX_OBJECTS) $(THEDATE)

# file name suffixes recognized by this Makefile:
.SUFFIXES: .o .C

##############################################################################
##                                                                          ##
##			D E P E N D E N C Y   R U L E S                     ##
##                                                                          ##
##############################################################################

# the default rule. Build the program if no target is specified:
default: program

# build the program:
program: $(PROGRAM)
$(PROGRAM): typeoutProgram rmdate $(SUBDIRS) $(OBJECTS)
	$(LD) $(LDFLAGS) \
	      $(OBJECTS) \
	      $(OLIBS) \
	      -o $@
	@( \
	   for othername in $(OTHERPROGRAMNAMES); \
	   do \
	        echo ln $@ $$othername ; \
	        rm -f $$othername ; ln $@ $$othername ; \
	   done)

# install the program in the INSTALLDIR:
install: checkmachine $(PROGRAM) $(INSTALLDIRS)
	@( \
	if [ -f $(INSTALLDIR)/$(PROGRAM) ] ; then \
	   echo mv $(INSTALLDIR)/$(PROGRAM) $(INSTALLDIR)/$(PROGRAM).old; \
	   mv $(INSTALLDIR)/$(PROGRAM) $(INSTALLDIR)/$(PROGRAM).old; \
	fi ; \
	echo cp $(PROGRAM) $(INSTALLDIR)/$(PROGRAM) ; \
	cp $(PROGRAM) $(INSTALLDIR)/$(PROGRAM) ; \
	for othername in $(OTHERPROGRAMNAMES); \
	do \
	    rm -f $(INSTALLDIR)/$$othername ; \
	    echo ln $(INSTALLDIR)/$(PROGRAM) $(INSTALLDIR)/$$othername ; \
	    ln $(INSTALLDIR)/$(PROGRAM) $(INSTALLDIR)/$$othername ; \
	done ; \
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
# to mail the splitted uuencode archive to anyone who cares to receive it:
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

# remove object files and the program:
clean: checkmachine
	rm -f $(OBJECTS) \
	      $(PROGRAM) $(OTHERPROGRAMNAMES) \
	      core

# remove object files, lex and yacc output and the program:
veryclean: checkmachine
	rm -f $(OBJECTS) \
	      $(PROGRAM) $(OTHERPROGRAMNAMES) \
	      core

# remove the program:
targetclean: checkmachine
	rm -f $(PROGRAM) $(OTHERPROGRAMNAMES)

# remove the distribution scratch directory and its contents:
distclean: checkmachine
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
	echo "rm -f $$CXX_SOURCES [mM]akefile thedate.[cC] $(HEADERS)" ; \
	rm -f $$CXX_SOURCES [mM]akefile thedate.[cC] $(HEADERS) ; \
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
	    "     % setenv MACHINE hp800 ; make\n\n" ; \
	    exit 1 ; \
	 fi

# three rules to make sure that thedate.c is always recompiled:
thedate: checkmachine rmdate $(THEDATE)
rmdate:; rm -f $(THEDATE)
$(THEDATE): checkmachine
	$(CXX) $(CXXFLAGS) \
	      "-DTHEDATE=\"`date | awk '{print $$1,$$2,$$3,$$NF}'`\"" \
	      "-DTHEHOST=\"`hostname`\"" \
	      "-DTHEMAN=\"`whoami`\"" \
	      -c $(@F:.o=.C) -o $@

# macros that expand to compilation commands for C++ sources:
DOCXX  = $(CXX) $(CXXFLAGS) -c $(@F:.o=.C) -o $@

# the final dependency for all the C-sources:
$(CXX_OBJECTS): $(OBJDIR)/%.o: %.C $(HEADERS)
	$(DOCXX)

##############################################################################
##                                                                          ##
##			 E N D   O F   M A K E F I L E                      ##
##                                                                          ##
##############################################################################
