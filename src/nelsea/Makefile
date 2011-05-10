#	@(#)Makefile 3.51 09/01/99 TU-Delft
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
PROGRAM = $(BINDIR)/nelsea

# other names for this executable:
OTHERPROGRAMNAMES = $(BINDIR)/fish $(BINDIR)/ghoti $(BINDIR)/sea

# the name of the archive to contain the object files:
ARCHIVE = $(LIBDIR)/libnelsea$(NELSIS_RELEASE).a

# the local header files:
HEADERS = \
	def.h \
	grid.h \
	nelsis.h \
	prototypes.h \
	seadifGraph.h \
	tbox.h \
	globals.h \
	typedef.h \
	dmescape.h 

# objects for ANSI-C compilation. For each object $(OBJDIR)/*.o there
# must exist a corresponding *.c file in the current directory:
C_OBJECTS = \
	  $(OBJDIR)/alloc.o \
	  $(OBJDIR)/error.o \
	  $(OBJDIR)/globals.o \
	  $(OBJDIR)/grid_globals.o \
	  $(OBJDIR)/tin_table.o \
	  $(OBJDIR)/csi_table.o \
	  $(OBJDIR)/hostname.o \
	  $(OBJDIR)/initialize.o \
	  $(OBJDIR)/makelib.o \
	  $(OBJDIR)/mapfile.o \
	  $(OBJDIR)/nelsis.o \
	  $(OBJDIR)/powerFix.o \
	  $(OBJDIR)/read_cell.o \
	  $(OBJDIR)/read_circuit.o \
	  $(OBJDIR)/read_layout.o \
	  $(OBJDIR)/read_seadif.o \
	  $(OBJDIR)/sea.o \
	  $(OBJDIR)/writ_circuit.o \
	  $(OBJDIR)/write_layout.o \
	  $(OBJDIR)/write_seadif.o \
	  $(OBJDIR)/dmescape.o

# objects for C++ compilation. For each object $(OBJDIR)/*.o there
# must exist a corresponding *.C file in the current directory:
CXX_OBJECTS = \
	$(OBJDIR)/main.o \
	$(OBJDIR)/seadifGraph.o \
	$(OBJDIR)/ghotiDelete.o \
	$(OBJDIR)/ghoti.o \
	$(OBJDIR)/serPar.o \
	$(OBJDIR)/runprog.o

# object that contains the date of compilation:
THEDATE = $(OBJDIR)/thedate.o

# the source file to compile with YACC:
YSOURCE = image.y

# the source file to compile with LEX:
LSOURCE = image.l

# by default, all the lex and yacc identifiers use a "yy" tag. This causes
# confusion and name clashes if more than one lex and yacc parser is being
# used. The macro YYNAME can be set here to something else than "yy".
# Do *NOT* set this to a single "y":
YYNAME = yd

# the root directory of the ocean tree:
OCEAN = /usr1/ocean

# directory where the ocean header files live:
SEAINCLDIR = $(OCEAN)/include

# directory where the ocean libraries live:
SEALIBDIR = $(OCEAN)/lib/${MACHINE}

# linker options to include ocean libraries:
SEALIBS = -locean -lseadif

# for which nelsis release are we compiling?
NELSIS_RELEASE = 3

# the root directory of the nelsis tree:
ICDPATH = /u/25/25/space2/cacd3

# directory where the nelsis header files live:
ICDINCLDIR = $(ICDPATH)/lib/include

# directory where the nelsis libraries live:
ICDLIBDIR = $(ICDPATH)/lib/$(MACHINE)

# linker options to include nelsis libraries:
ICDLIBS = -llayfmt -lcirfmt -lflpfmt -lddm

# linker options to include all required libraries:
OLIBS = $(SEALIBS) $(ICDLIBS) $(XOLIBS)

# eXtra OLIBS:
XOLIBS = -ll

# eXtra flags for the ANSI-C compiler:
XCFLAGS= -O

# options for the ANSI-C compiler:
CFLAGS = -I$(SEAINCLDIR) -I$(ICDINCLDIR) -DNELSIS_REL$(NELSIS_RELEASE) -DANNOTATE

# the C++ compiler:
CXX = CC

# the ANSI-C compiler:
CC = cc

# eXtra flags for C++ compilation:
XCXXFLAGS = $(XCFLAGS)

# options for the C++ compiler:
CXXFLAGS = -I$(SEAINCLDIR) -I$(ICDINCLDIR) -DNELSIS_REL$(NELSIS_RELEASE)

# AvG
# the linker:
LD = $(CXX)        # Since we have C++ objects, the linker must also be C++

# options for the linker (does NOT include de -l options in OLIBS):
LDFLAGS = -L$(SEALIBDIR) -L$(ICDLIBDIR) $(XLDFLAGS)

# eXtra load flags:
XLDFLAGS = $(XCXXFLAGS)

# the flags used to compile the YACCOUTPUT:
LYCFLAGS = $(CFLAGS) -DHAVE_$(LEX)

# eXtra flags for compiling the lex/yacc source:
XLYCFLAGS = 

# the lexical analyzer generator:
LEX = flex

# options for LEX:
LEXFLAGS = 

# the parser generator:
YACC = yacc++

# options for YACC:
YACCFLAGS = 

# the archive program:
AR = ar

# options for AR:
ARFLAGS = ruv

# ranlib
RANLIB = ranlib

# directory to install the PROGRAM and the ARCHIVE:
INSTALLDIR = $(OCEAN)

# some targets (noticeably checkout_and_install) may call an inferior "make".
# This defines which macros will be inherited by the inferior make process:
INHERIT_MACROS = \
	"OCEAN=$(OCEAN)" \
	"ICDPATH=$(ICDPATH)" \
	"INSTALLDIR=$(INSTALLDIR)" \
	"NELSIS_RELEASE=$(NELSIS_RELEASE)" \
	"XCFLAGS=$(XCFLAGS)"

##############################################################################
##                                                                          ##
##			     S T A T I C   A R E A                          ##
##                                                                          ##
##############################################################################

# the shell to exec the command lines in this Makefile. Should be /bin/sh:
SHELL = /bin/sh

# subdirectories to contain the objects, the archive and the executable:
OBJDIR = objects/$(MACHINE)/nelsis$(NELSIS_RELEASE)
LIBDIR = lib/$(MACHINE)
BINDIR = bin/$(MACHINE)/nelsis$(NELSIS_RELEASE)

# a scratch directory to check-out, build and install the latest version:
BUILDDIR = TMP.install

# a scratch dir to check-out the latest version and build a tar archive:
DISTRIBUTIONDIR = TMP.distrib

# All subdirectories that must exist before attempting compilation. In
# this list parent directories must appear before descendant directories:
SUBDIRS = objects objects/$(MACHINE) $(OBJDIR) \
          lib $(LIBDIR) \
          bin bin/$(MACHINE) $(BINDIR)

# All subdirectories of the INSTALLDIR that must exist before attempting
# installation. In this list parent directories must appear before descendant
# directories:
INSTALLDIRS = $(INSTALLDIR) \
	      $(INSTALLDIR)/bin \
	      $(INSTALLDIR)/bin/$(MACHINE) \
	      $(INSTALLDIR)/$(BINDIR) \
	      $(INSTALLDIR)/lib \
	      $(INSTALLDIR)/$(LIBDIR)

# all the objects zusammen:
OBJECTS = $(C_OBJECTS) $(CXX_OBJECTS) $(LYOBJECT) $(THEDATE)

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

# build the archive:
archive: $(ARCHIVE)
$(ARCHIVE): typeoutArchive $(SUBDIRS) \
            $(C_OBJECTS) $(CXX_OBJECTS) $(LYOBJECT) $(THEDATE)
	$(AR) $(ARFLAGS) $(ARCHIVE) \
	      $(C_OBJECTS) $(CXX_OBJECTS) $(LYOBJECT) $(THEDATE)
	$(RANLIB) $(ARCHIVE)

# install the program and the archive in the INSTALLDIR:
install: checkmachine $(PROGRAM) $(ARCHIVE) $(INSTALLDIRS)
	@( \
	  if [ -f $(INSTALLDIR)/$(PROGRAM) ] ; then \
	     echo mv -f $(INSTALLDIR)/$(PROGRAM) $(INSTALLDIR)/$(PROGRAM).old; \
	     mv -f $(INSTALLDIR)/$(PROGRAM) $(INSTALLDIR)/$(PROGRAM).old; \
	  fi ; \
	  echo cp $(PROGRAM) $(INSTALLDIR)/$(PROGRAM) ; \
	  cp $(PROGRAM) $(INSTALLDIR)/$(PROGRAM) ; \
	  for othername in $(OTHERPROGRAMNAMES); \
	  do \
	      rm -f $(INSTALLDIR)/$$othername ; \
	      echo ln $(INSTALLDIR)/$(PROGRAM) $(INSTALLDIR)/$$othername ; \
	      ln $(INSTALLDIR)/$(PROGRAM) $(INSTALLDIR)/$$othername ; \
	  done ; \
	  echo rm -f $(INSTALLDIR)/$(ARCHIVE) ; \
	  rm -f $(INSTALLDIR)/$(ARCHIVE) ; \
	  echo cp $(ARCHIVE) $(INSTALLDIR)/$(ARCHIVE) ; \
	  cp $(ARCHIVE) $(INSTALLDIR)/$(ARCHIVE) ; \
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

# remove object files, the program and the archive:
clean: checkmachine
	rm -f $(OBJECTS) \
	      $(PROGRAM) $(OTHERPROGRAMNAMES) \
	      $(ARCHIVE) core

# remove object files, lex and yacc output, the program and the archive:
veryclean: checkmachine
	rm -f $(OBJECTS) \
	      $(YACCOUTPUT) $(LEXOUTPUT) \
	      $(PROGRAM) $(OTHERPROGRAMNAMES) \
	      $(ARCHIVE) core

# remove lex and yacc output files:
yyclean: checkmachine
	rm -f $(LYOBJECT) $(YACCOUTPUT) $(LEXOUTPUT)

# remove the program and the archive:
targetclean: checkmachine
	rm -f $(ARCHIVE) $(PROGRAM) $(OTHERPROGRAMNAMES)

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
	C_SOURCES=`echo $(C_OBJECTS) | sed -e 's;[^ ]*/;;g' -e 's/\.o/.c/g'` ; \
	echo "rm -f $$CXX_SOURCES $$C_SOURCES [mM]akefile thedate.[cC] $(HEADERS) $(YSOURCE) $(LSOURCE)" ; \
	rm -f $$CXX_SOURCES $$C_SOURCES [mM]akefile thedate.[cC] $(HEADERS) $(YSOURCE) $(LSOURCE) ; \
	)

# compile an object file from the lex and yacc output files:
$(LYOBJECT): $(YACCOUTPUT) $(LEXOUTPUT) $(HEADERS)
	@( \
	echo "" ; \
	SOURCEDIR=`pwd` ; \
	echo cd $(@D) ; cd $(@D) ; \
	echo $(CC) $(LYCFLAGS) $(XLYCFLAGS) -I$$SOURCEDIR -I. -c $(@F:.o=.c) -o $(@F) ; \
	$(CC) $(LYCFLAGS) $(XLYCFLAGS) -I$$SOURCEDIR -I. -c $(@F:.o=.c) -o $(@F) ; \
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
	echo "sed -e 's/yy/$(YYNAME)/g' -e 's/$(YYNAME)wrap/yywrap/g' lex.yy.c > $(@F)" ; \
	sed -e 's/yy/$(YYNAME)/g' \
	    -e 's/$(YYNAME)wrap/yywrap/g' lex.yy.c > $(@F) ; \
	echo rm -f lex.yy.c ; \
	rm -f lex.yy.c ; \
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

# three rules to make sure that thedate.c is always recompiled:
thedate: checkmachine rmdate $(THEDATE)
rmdate:; rm -f $(THEDATE)
$(THEDATE): checkmachine
	$(CC) $(CFLAGS) $(XCFLAGS) \
	      "-DTHEDATE=\"`date | awk '{print $$1,$$2,$$3,$$NF}'`\"" \
	      "-DTHEHOST=\"`hostname`\"" \
	      "-DTHEMAN=\"`whoami`\"" \
	      -c $(@F:.o=.c) -o $@

# macros that expand to compilation commands for C and C++ sources:
DOCC  =	$(CC) $(CFLAGS) $(XCFLAGS) -c $(@F:.o=.c) -o $@
DOCXX =	$(CXX) $(CXXFLAGS) $(XCXXFLAGS) -c $(@F:.o=.C) -o $@

# the final dependency for all the C-sources:
$(C_OBJECTS): $(OBJDIR)/%.o: %.c $(HEADERS)
	$(DOCC)

# the final dependency for all the C++ sources:
$(CXX_OBJECTS): $(OBJDIR)/%.o: %.C $(HEADERS)
	$(DOCXX)

##############################################################################
##                                                                          ##
##			 E N D   O F   M A K E F I L E                      ##
##                                                                          ##
##############################################################################
