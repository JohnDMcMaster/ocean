#	@(#)Makefile 1.13 10/31/94 Delft University of Technology
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

# the name of the archive to contain the object files:
ARCHIVE = $(LIBDIR)/liblia.a

# the local header files:
HEADERS = \
	array.h \
	basearr.h \
	box.h \
	item.h \
	list.h \
	lstelem.h \
	package.h

# objects for C++ compilation. For each object $(OBJDIR)/*.o there
# must exist a corresponding *.C file in the current directory:
CXX_OBJECTS = \
	$(OBJDIR)/array.o \
	$(OBJDIR)/basearr.o \
	$(OBJDIR)/box.o \
	$(OBJDIR)/item.o \
	$(OBJDIR)/list.o \
	$(OBJDIR)/package.o

# the root directory of the ocean tree:
OCEAN = /usr1/ocean

# directory where the ocean header files live:
SEAINCLDIR = $(OCEAN)/include

# overide at the command line (XCFLAGS=-g) if you want debugging information:
XCFLAGS= -O

# the C++ compiler:
CXX = CC

# options for the C++ compiler:
CXXFLAGS = $(XCFLAGS) -I. -I$(SEAINCLDIR)

# the archive program:
AR = ar

# options for AR:
ARFLAGS = ruv

# how to build a symbol table for the archive:
RANLIB = ranlib

# some targets (noticeably checkout_and_install) may call an inferior "make".
# This defines which macros will be inherited by the inferior make process:
INHERIT_MACROS = \
	"OCEAN=$(OCEAN)" \
	"LIBDIR=$(LIBDIR)" \
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

# all the objects zusammen:
OBJECTS = $(CXX_OBJECTS)

# file name suffixes recognized by this Makefile:
.SUFFIXES: .o .c .y .l .C

##############################################################################
##                                                                          ##
##			D E P E N D E N C Y   R U L E S                     ##
##                                                                          ##
##############################################################################

# the default rule. Build the archive if no target is specified:
default: archive

# build the archive:
archive: $(ARCHIVE)
$(ARCHIVE): typeoutArchive $(SUBDIRS) $(OBJECTS)
	$(AR) $(ARFLAGS) $(ARCHIVE) $(OBJECTS)
	$(RANLIB) $(ARCHIVE)

# install the program and the archive in the INSTALLDIR:
install:
	@echo "\n    INSTALL does not make sense for `basename $(ARCHIVE)`\n"
	@/bin/false

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
	C_SOURCES=`echo $(C_OBJECTS) | sed -e 's;[^ ]*/;;g' -e 's/\.o/.c/g'` ; \
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

# macros that expand to compilation commands for C++ and ANSI-C sources:
DOCXX =	$(CXX) $(CXXFLAGS) -c $(@F:.o=.C) -o $@

# the final dependency for all the C++ sources:
$(CXX_OBJECTS): $(OBJDIR)/%.o: %.C $(HEADERS)
	$(DOCXX)

##############################################################################
##                                                                          ##
##			 E N D   O F   M A K E F I L E                      ##
##                                                                          ##
##############################################################################
