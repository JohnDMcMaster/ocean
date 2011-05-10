#	@(#)Makefile 1.24 10/31/94 Delft University of Technology
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
ARCHIVE = $(LIBDIR)/libphil.a

# the local header files:
HEADERS = \
	clst.h \
	cluster.h \
	cri.h \
	cricand.h \
	divTable.h \
	im.h \
	image.h \
	imageMap.h \
	layMap.h \
	parserIf.h \
	pattern.h \
	phil.h \
	phil_glob.h \
	plane.h \
	plaza.h \
	plcm.h \
	point.h \
	protArea.h \
	sea.h \
	usrlib.h \
	window.h

# objects for C++ compilation. For each object $(OBJDIR)/*.o there
# must exist a corresponding *.C file in the current directory:
CXX_OBJECTS = \
	$(OBJDIR)/clst.o \
	$(OBJDIR)/cluster.o \
	$(OBJDIR)/compact.o \
	$(OBJDIR)/cri.o \
	$(OBJDIR)/divTable.o \
	$(OBJDIR)/imageMap.o \
	$(OBJDIR)/layMap.o \
	$(OBJDIR)/myfree.o \
	$(OBJDIR)/parserIf.o \
	$(OBJDIR)/pattern.o \
	$(OBJDIR)/phil.o \
	$(OBJDIR)/plane.o \
	$(OBJDIR)/plcm.o \
	$(OBJDIR)/plcminit.o \
	$(OBJDIR)/point.o \
	$(OBJDIR)/protArea.o \
	$(OBJDIR)/usrlib.o \
	$(OBJDIR)/window.o \
	$(OBJDIR)/debug.o \
	$(OBJDIR)/mtx.o \
	$(OBJDIR)/stuff.o \
	$(OBJDIR)/initimagedsc.o

# objects for ANSI-C compilation. For each object $(OBJDIR)/*.o there
# must exist a corresponding *.c file in the current directory:
C_OBJECTS =

# the source file to compile with YACC:
YSOURCE = image.y

# the source file to compile with LEX:
LSOURCE = image.l

# by default, all the lex and yacc identifiers use a "yy" tag. This causes
# confusion and name clashes if more than one lex and yacc parser is being
# used. The macro YYNAME can be set here to something else than "yy".
# Do *NOT* set this to a single "y":
YYNAME = pp

# the root directory of the ocean tree:
OCEAN = /usr1/ocean

# directory where the ocean header files live:
SEAINCLDIR = $(OCEAN)/include

# directory where the lia and the partitioner header files live:
LIADIR = ../lia
PARTITIONDIR = ../partitioner

# overide at the command line (XCFLAGS=-g) if you want debugging information:
XCFLAGS= -O

# the ANSI-C compiler:
CC = CC

# options for the ANSI-C compiler:
CFLAGS = $(XCFLAGS) -I$(SEAINCLDIR) -I$(LIADIR) -I$(PARTITIONDIR)

# the C++ compiler:
CXX = CC

# options for the C++ compiler:
CXXFLAGS = $(XCFLAGS) -I. -I$(SEAINCLDIR) -I$(LIADIR) -I$(PARTITIONDIR)

# the flags used to compile the YACCOUTPUT:
LYCFLAGS = $(CFLAGS) -DHAVE_$(LEX) -I../../$(LIADIR) -I../../$(PARTITIONDIR)

# the lexical analyzer generator:
LEX = flex

# options for LEX:
LEXFLAGS = -F

# the parser generator:
YACC = yacc++

# options for YACC.  Set this to -l if you want to use the symbolic debugger
# with yacc generated C-code. (In this case, do not forget to add -g to the
# CFLAGS.) Remove the -l option if you still have syntax errors in the yacc
# source:
YACCFLAGS = 

# the archive program:
AR = ar

# options for AR:
ARFLAGS = ruv

# how to build a symbol table for the archive:
RANLIB = ranlib

# some targets (noticeably checkout_and_install) may call an inferior "make".
# This defines which macros will be inherited by the inferior make process.
# Use the construct $${SOURCEDIR} to refer to the current source directory:
INHERIT_MACROS = \
	"OCEAN=$(OCEAN)" \
	"LIBDIR=$${SOURCEDIR}$(LIBDIR)" \
	"CXX=$(CXX)" \
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
SUBDIRS = objects $(OBJDIR) lib $(LIBDIR) bin $(BINDIR)

# all the objects zusammen:
OBJECTS = $(CXX_OBJECTS) $(C_OBJECTS) $(LYOBJECT)

# it is assumed that the output from yacc (default y.tab.c) #include's the
# output from lex, so that we do not have to compile the LEXOUTPUT separately.
# LYOBJECT is the name of this combined lex/yacc object file; implicitly the
# name of the YACC output file is $(LYOBJECT:.o=.c) ...:
LYOBJECT  = $(OBJDIR)/$(YYNAME).tab.o
YACCOUTPUT = $(OBJDIR)/$(YYNAME).tab.c # stupid, but make has documented bug...
# the desired name of the lex output file. This must be #include'd by YSOURCE:
LEXOUTPUT = $(OBJDIR)/lex.$(YYNAME).c

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

# compile an object file from the lex and yacc output files:
$(LYOBJECT): $(YACCOUTPUT) $(LEXOUTPUT) $(HEADERS)
	@( \
	echo "" ; \
	SOURCEDIR=`pwd` ; \
	echo cd $(@D) ; cd $(@D) ; \
	echo $(CXX) $(LYCFLAGS) -I$$SOURCEDIR -I. -c $(@F:.o=.c) -o $(@F) ; \
	$(CXX) $(LYCFLAGS) -I$$SOURCEDIR -I. -c $(@F:.o=.c) -o $(@F) ; \
	)

# run yacc in the object subdirectory:
$(YACCOUTPUT): $(YSOURCE)
	@( \
	echo "\nCreating $@ ..." ; \
	SOURCEDIR=`pwd` ; \
	echo cd $(@D) ; cd $(@D) ; \
	echo $(YACC) $(YACCFLAGS) $$SOURCEDIR/$(YSOURCE) ; \
	$(YACC) $(YACCFLAGS) $$SOURCEDIR/$(YSOURCE) ; \
	echo "sed -e 's/yy/$(YYNAME)/g' -e 's/$(YYNAME)wrap/yywrap/g' y.tab.c > $(@F)" ; \
	sed -e 's/yy/$(YYNAME)/g' \
	    -e 's/$(YYNAME)wrap/yywrap/g' y.tab.c > $(@F) ; \
	echo rm -f y.tab.c ; \
	rm -f y.tab.c ; \
	)

# run lex in the object subdirectory:
$(LEXOUTPUT): $(LSOURCE)
	@( \
	echo "\nCreating $@ ..." ; \
	SOURCEDIR=`pwd` ; \
	echo cd $(@D) ; cd $(@D) ; \
	echo $(LEX) $(LEXFLAGS) $$SOURCEDIR/$(LSOURCE) ; \
	$(LEX) $(LEXFLAGS) $$SOURCEDIR/$(LSOURCE) ; \
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
DOCC =	$(CC) $(CFLAGS) -c $(@F:.o=.c) -o $@

# the final dependency for all the C++ sources:
$(CXX_OBJECTS): $(OBJDIR)/%.o: %.C $(HEADERS)
	$(DOCXX)

# the final dependency for all the ANSI-C sources:
#$(C_OBJECTS): $(OBJDIR)/%.o: %.c $(HEADERS)
#	$(DOCC)

##############################################################################
##                                                                          ##
##			 E N D   O F   M A K E F I L E                      ##
##                                                                          ##
##############################################################################
