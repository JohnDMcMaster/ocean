#	@(#)Makefile 1.7 (Delft University of Technology) 12/10/94
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
PROGRAM = $(BINDIR)/esea

# other names for this executable:
OTHERPROGRAMNAMES = $(BINDIR)/$(NELSIS)/cedif

# name of the manual page
MANPAGE = cedif.1

# alternative names for the manual page
OTHERMANPAGENAMES = esea.1

# the root directory of the ocean tree:
OCEAN = /usr1/ocean

# the local header files:
HEADERS = \
	cedif.h \
	thedate.h

# objects for ANSI-C compilation. For each object $(OBJDIR)/*.o there
# must exist a corresponding *.c file in the current directory:
C_OBJECTS = \
	$(OBJDIR)/main.o \
	$(OBJDIR)/globals.o \
	$(OBJDIR)/nelsisTarget.o \
	$(OBJDIR)/seadifTarget.o \
	$(OBJDIR)/report.o \
	$(OBJDIR)/solveRef.o

# the source file to compile with YACC:
YSOURCE = edif.y

# the source file to compile with LEX:
LSOURCE = edif.l

# by default, all the lex and yacc identifiers use a "yy" tag. This causes
# confusion and name clashes if more than one lex and yacc parser is being
# used. The macro YYNAME can be set here to something else than "yy".
# Do *NOT* set this to a single "y":
YYNAME = edif

# object that contains the date of compilation:
THEDATE = $(OBJDIR)/thedate.o

# directory where the ocean manual pages live:
SEAMANDIR = $(OCEAN)/man

# directory where the ocean header files live:
SEAINCLDIR = $(OCEAN)/include

# directory where the ocean libraries live:
SEALIBDIR = $(OCEAN)/lib/${MACHINE}

# linker options to include ocean libraries:
SEALIBS = -lseadif

# the root directory of the nelsis tree:
ICDPATH = /usr/cacd

# directory where the nelsis header files live:
ICDINCLDIR = $(ICDPATH)/lib/include

# directory where the nelsis libraries live:
ICDLIBDIR = $(ICDPATH)/lib/${MACHINE}

# the nelsis release (either 3 or 4):
NELSIS_REL = 3

# full nelsis release:
NELSIS = nelsis$(NELSIS_REL)

# linker options to include nelsis libraries:
ICDLIBS = -llayfmt -lcirfmt -lflpfmt -lddm

# linker options to include all required libraries:
OLIBS = $(SEALIBS) $(ICDLIBS)

# overide at the command line (XCFLAGS=-g) if you want debugging information:
XCFLAGS= -O

# the C++ compiler:
CXX = g++

# options for the C++ compiler:
CXXFLAGS = $(XCFLAGS) -I$(SEAINCLDIR)

# the ANSI-C compiler:
CC = gcc

# options for the ANSI-C compiler:
CFLAGS = -I$(SEAINCLDIR) -I$(ICDINCLDIR)

# the linker:
LD = $(CC)

# eXtra flags for the linker:
XLDFLAGS = $(XCFLAGS)

# options for the linker (does NOT include de -l options in OLIBS):
LDFLAGS = $(XLDFLAGS) -L$(SEALIBDIR) -L$(ICDLIBDIR)

# directory to install the PROGRAM:
INSTALLDIR = $(OCEAN)

# the lexical analyzer generator:
LEX = flex

# options for LEX:
LEXFLAGS = -i

# the parser generator:
YACC = bison -y

# options for YACC.  Set this to -l if you want to use the symbolic debugger
# with yacc generated C-code. (In this case, do not forget to add -g to the
# XLYCFLAGS.) Remove the -l option if you still have syntax errors in the yacc
# source:
YACCFLAGS = -d

# C flags for compiling the lex/yacc generated sources:
LYCFLAGS  = $(XLYCFLAGS) -I$(SEAINCLDIR)

# eXtra flags for compilitation of the lex/yacc generated sources:
XLYCFLAGS = $(XCFLAGS)

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
SUBDIRS = objects $(OBJDIR) bin $(BINDIR) $(BINDIR)/$(NELSIS)

# All subdirectories of the INSTALLDIR that must exist before attempting
# installation. In this list parent directories must appear before descendant
# directories:
INSTALLDIRS = $(INSTALLDIR) \
	      $(SEAMANDIR) \
	      $(SEAMANDIR)/man1 \
	      $(INSTALLDIR)/bin \
	      $(INSTALLDIR)/$(BINDIR)

# all the objects zusammen:
OBJECTS = $(C_OBJECTS) $(YOBJECT) $(LOBJECT) $(THEDATE)

# names of the yacc and lex output files:
LEXOUTPUT  = $(OBJDIR)/lex.$(YYNAME).c
YACCOUTPUT = $(OBJDIR)/$(YYNAME).tab.c
YACCHEADER = $(YACCOUTPUT:.c=.h)

# the names of the lex/yacc objects after compilation with $(CC):
LOBJECT    = $(LEXOUTPUT:.c=.o)
YOBJECT    = $(YACCOUTPUT:.c=.o)

# file name suffixes recognized by this Makefile:
.SUFFIXES: .o .C .l .y .c

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
	@for prog in $(OTHERPROGRAMNAMES); do \
	    rm -f $$prog ; \
	    echo ln $@ $$prog; ln $@ $$prog; \
	 done

# install the program in the INSTALLDIR:
install: checkmachine $(PROGRAM) $(INSTALLDIRS)
	@( \
	if [ -f $(INSTALLDIR)/$(PROGRAM) ] ; then \
	   echo mv $(INSTALLDIR)/$(PROGRAM) $(INSTALLDIR)/$(PROGRAM).old; \
	   mv $(INSTALLDIR)/$(PROGRAM) $(INSTALLDIR)/$(PROGRAM).old; \
	fi ; \
	echo cp $(PROGRAM) $(INSTALLDIR)/$(PROGRAM) ; \
	cp $(PROGRAM) $(INSTALLDIR)/$(PROGRAM) ; \
	for prog in $(OTHERPROGRAMNAMES); do \
	    rm -f $(INSTALLDIR)/$$prog ; \
	    echo ln $(INSTALLDIR)/$(PROGRAM) $(INSTALLDIR)/$$prog ; \
	    ln $(INSTALLDIR)/$(PROGRAM) $(INSTALLDIR)/$$prog ; \
	done ; \
	echo cp $(MANPAGE) $(SEAMANDIR)/man1 ; \
	rm -f $(SEAMANDIR)/man1/$(MANPAGE) ; \
	cp $(MANPAGE) $(SEAMANDIR)/man1 ; \
	cd $(SEAMANDIR)/man1 ; \
	for mp in $(OTHERMANPAGENAMES); do \
	    echo ln $(MANPAGE) $$mp; rm -f $$mp ; ln $(MANPAGE) $$mp; \
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
	      $(YACCOUTPUT) $(LEXOUTPUT) $(YACCHEADER) \
	      $(PROGRAM) $(OTHERPROGRAMNAMES) \
	      core

# remove lex and yacc output files:
yyclean: checkmachine
	rm -f $(LOBJECT) $(YOBJECT) $(YACCOUTPUT) $(LEXOUTPUT) $(YACCHEADER)

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
	C_SOURCES=`echo $(C_OBJECTS) | sed -e 's;[^ ]*/;;g' -e 's/\.o/.C/g'` ; \
	echo "rm -f $$C_SOURCES [mM]akefile thedate.[cC] $(HEADERS)" ; \
	rm -f $$C_SOURCES [mM]akefile thedate.[cC] $(HEADERS) ; \
	)

# compile an object file from the lex and yacc output files:
$(LOBJECT) $(YOBJECT): $(OBJDIR)/%.o : $(OBJDIR)/%.c $(HEADERS)
	@( \
	echo "" ; \
	SOURCEDIR=`pwd` ; \
	echo cd $(@D) ; cd $(@D) ; \
	echo $(CC) $(LYCFLAGS) -I$$SOURCEDIR -I. -c $(@F:.o=.c) -o $(@F) ; \
	$(CC) $(LYCFLAGS) -I$$SOURCEDIR -I. -c $(@F:.o=.c) -o $(@F) ; \
	status=$$? ; echo "" ; exit $$status \
	)

# run yacc in the object subdirectory:
$(YACCOUTPUT): $(YSOURCE)
	@( \
	echo "\nCreating $@ ..." ; \
	SOURCEDIR=`pwd` ; \
	echo cd $(@D) ; cd $(@D) ; \
	echo $(YACC) $(YACCFLAGS) $$SOURCEDIR/$(YSOURCE) ; \
	$(YACC) $(YACCFLAGS) $$SOURCEDIR/$(YSOURCE) ; \
	if [ $$? != 0 ] ; then exit $$?; fi ; \
	echo "sed -e 's/yy/$(YYNAME)/g' -e 's/$(YYNAME)wrap/yywrap/g' y.tab.c > $(@F)" ; \
	sed -e 's/yy/$(YYNAME)/g' \
	    -e 's/$(YYNAME)wrap/yywrap/g' y.tab.c > $(@F) ; \
	echo rm -f y.tab.c ; \
	rm -f y.tab.c ; \
	if [ -f y.tab.h ] ; then \
	   echo "sed -e 's/yy/$(YYNAME)/g' y.tab.h > $(YYNAME).tab.h" ; \
	   sed -e 's/yy/$(YYNAME)/g' y.tab.h > $(YYNAME).tab.h ; \
	   echo rm y.tab.h ; rm y.tab.h ; \
	fi ; \
	exit 0 \
	)

# run lex in the object subdirectory:
$(LEXOUTPUT): $(LSOURCE) $(YACCOUTPUT)
	@( \
	echo "\nCreating $@ ..." ; \
	SOURCEDIR=`pwd` ; \
	echo cd $(@D) ; cd $(@D) ; \
	echo $(LEX) $(LEXFLAGS) $$SOURCEDIR/$(LSOURCE) ; \
	$(LEX) $(LEXFLAGS) $$SOURCEDIR/$(LSOURCE) ; \
	if [ $$? != 0 ] ; then exit $$?; fi ; \
	mv lex.yy.c lex.y.c ; \
	echo "sed -e 's/yy/$(YYNAME)/g' -e 's/$(YYNAME)wrap/yywrap/g' lex.y.c > $(@F)" ; \
	sed -e 's/yy/$(YYNAME)/g' \
	    -e 's/$(YYNAME)wrap/yywrap/g' lex.y.c > $(@F) ; \
	echo rm -f lex.y.c ; \
	rm -f lex.y.c ; \
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

# three rules to make sure that thedate.C is always recompiled:
thedate: checkmachine rmdate $(THEDATE)
rmdate:; rm -f $(THEDATE)
$(THEDATE): checkmachine
	$(CC) $(CFLAGS) $(XCFLAGS) \
	      "-DTHEDATE=\"`date | awk '{print $$1,$$2,$$3,$$NF}'`\"" \
	      "-DTHEHOST=\"`hostname`\"" \
	      "-DTHEMAN=\"`whoami`\"" \
	      -c $(@F:.o=.c) -o $@

# macros that expand to compilation commands for ANSI-C sources:
DOCC  = $(CC) $(CFLAGS) $(XCFLAGS) -c $(@F:.o=.c) -o $@

# the final dependency for all the ANSI-C sources:
$(C_OBJECTS): $(OBJDIR)/%.o: %.c $(HEADERS)
	$(DOCC)

##############################################################################
##                                                                          ##
##			 E N D   O F   M A K E F I L E                      ##
##                                                                          ##
##############################################################################
