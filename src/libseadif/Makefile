#	@(#)Makefile 1.40 10/31/94 Delft University of Technology
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
ARCHIVE = $(LIBDIR)/libseadif.a

# the local header files:
GLBLHDRS = \
	sealibio.h \
	namelist.h \
	sdferrors.h \
	sea_decl.h \
	sea_func.h \
	sealib.h \
	syssig.h \
	systypes.h \
        libstruct.h \
        sysdep.h \
	tmio.h \
	tm_struct.h


HEADERS =  sm.h $(GLBLHDRS)

# objects for ANSI-C compilation. For each object $(OBJDIR)/*.o there
# must exist a corresponding *.c file in the current directory:
C_OBJECTS = \
	$(OBJDIR)/alias.o \
	$(OBJDIR)/defaultenv.o \
	$(OBJDIR)/delete.o \
	$(OBJDIR)/dumpdb.o \
	$(OBJDIR)/err.o \
	$(OBJDIR)/hashstring.o \
	$(OBJDIR)/imagedesc.o \
	$(OBJDIR)/sealibio.o \
	$(OBJDIR)/libnametoptr.o \
	$(OBJDIR)/memman.o \
	$(OBJDIR)/namelist.o \
	$(OBJDIR)/readdb.o \
	$(OBJDIR)/sdfreport.o \
	$(OBJDIR)/seaflat.o \
	$(OBJDIR)/signal.o \
	$(OBJDIR)/slicecleanup.o \
	$(OBJDIR)/sm.o \
	$(OBJDIR)/stv_table.o \
	$(OBJDIR)/support.o \
	$(OBJDIR)/symlink.o \
	$(OBJDIR)/timecvt.o

# main is seperate because we don't want to include it in the archive:
MAIN = $(OBJDIR)/main.o

# the source file to compile with YACC:
YSOURCE = seadif.y

# the source file to compile with LEX:
LSOURCE = seadif.l

# by default, all the lex and yacc identifiers use a "yy" tag. This causes
# confusion and name clashes if more than one lex and yacc parser is being
# used. The macro YYNAME can be set here to something else than "yy".
# Do *NOT* set this to a single "y":
YYNAME = seadif

# the root directory of the ocean tree:
OCEAN = /usr1/ocean

# directory where the ocean header files live:
SEAINCLDIR = $(OCEAN)/include

# directory where the ocean manual pages live:
SEAMANDIR = $(OCEAN)/man

# linker options to include all required libraries:
OLIBS = $(XOLIBS)

# eXtra OLIBS:
XOLIBS = -ll

# overide at the command line (XCFLAGS=-g) if you want debugging information:
XCFLAGS= -O

# the ANSI-C compiler:
CC = cc -Aa -D_HPUX_SOURCE -D__STDC__

# options for the ANSI-C compiler:
CFLAGS = -I. -DHAVE_$(LEX)

BIGCC = $(CC)
# On the HP9000/300 series, the flex -F and the cc -O combination
# make the compiler say ``C1 internal error in "yyparse": Out of Memory''
# On this machine, you should uncomment the next line:
# BIGCC = cc +Nw600 +Ns5000

# the flags used to compile the YACCOUTPUT:
LYCFLAGS = $(CFLAGS) $(XLYCFLAGS)

# eXtra flags for compiling the lex/yacc source:
XLYCFLAGS = 

# the linker:
LD = $(CC)

# eXtra flags for the linker:
XLDFLAGS = $(XCFLAGS)

# options for the linker (does NOT include de -l options in OLIBS):
LDFLAGS = $(XLDFLAGS)

# the lexical analyzer generator:
LEX = flex

# options for LEX:
#LEXFLAGS = -F
LEXFLAGS = 

# the parser generator:
YACC = yacc

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

# directory to install the PROGRAM and the ARCHIVE:
INSTALLDIR = $(OCEAN)

# the manual pages that document this thing:
MANPAGES = \
	cs.3 \
	memman.3 \
	sdfattachli.3 \
	sdfclose.3 \
	sdfdelete.3 \
	sdfexists.3 \
	sdfflatcir.3 \
	sdfgetcwd.3 \
	sdflist.3 \
	sdflistall.3 \
	sdfmakeshapef.3 \
	sdfopen.3 \
	sdfread.3 \
	sdfreadall.3 \
	sdfremove.3 \
	sdftouch.3 \
	sdfwrite.3 \
	sdfwriteall.3 \
	sealib.3 \
	seadif.4

# some targets (noticeably checkout_and_install) may call an inferior "make".
# This defines which macros will be inherited by the inferior make process:
INHERIT_MACROS = \
	"INSTALLDIR=$(INSTALLDIR)" \
	"XCFLAGS=$(XCFLAGS)" \
	"XLYCFLAGS=$(XLYCFLAGS)" \
	"LEX=$(LEX)" \
	"YACC=$(YACC)"

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
	      $(INSTALLDIR)/include \
	      $(INSTALLDIR)/$(MANDIR) \
	      $(INSTALLDIR)/$(MANDIR)/man3 \
	      $(INSTALLDIR)/$(MANDIR)/man4

# all the objects zusammen:
OBJECTS = $(C_OBJECTS) $(LYOBJECT)

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

# build the program:
libtest: program
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

# install the archive and the header files in the INSTALLDIR:
install: checkmachine $(ARCHIVE) $(MANPAGES) $(INSTALLDIRS)
	rm -f $(INSTALLDIR)/$(ARCHIVE)
	cp $(ARCHIVE) $(INSTALLDIR)/$(ARCHIVE)
	@( \
	 set +e ; \
	 echo installing global header files if necessary ... ; \
	 for f in $(GLBLHDRS); do \
	     if [ ! -f $(SEAINCLDIR)/$$f ] ; then \
	        echo "\tcp $$f $(SEAINCLDIR)/$$f"; \
	        rm -f $(SEAINCLDIR)/$$f; \
	        cp $$f $(SEAINCLDIR)/$$f; \
	     elif cmp -s $(SEAINCLDIR)/$$f $$f; then \
	        true ; \
	     else \
		echo "\tcp $$f $(SEAINCLDIR)/$$f"; \
	        rm -f $(SEAINCLDIR)/$$f; \
		cp $$f $(SEAINCLDIR)/$$f; \
	     fi ; \
	 done ; \
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
	rm -f $(OBJECTS) $(MAIN) \
	      $(PROGRAM) \
	      $(ARCHIVE) core

# remove object files, lex and yacc output, the program and the archive:
veryclean: checkmachine
	rm -f $(OBJECTS) $(MAIN) \
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
	echo $(BIGCC) $(LYCFLAGS) -I$$SOURCEDIR -I. -c $(@F:.o=.c) -o $(@F) ; \
	$(BIGCC) $(LYCFLAGS) -I$$SOURCEDIR -I. -c $(@F:.o=.c) -o $(@F) || exit 1; \
	echo "" ; \
	)

# run yacc in the object subdirectory:
$(YACCOUTPUT): $(YSOURCE)
	@( \
	echo "\nCreating $@ ..." ; \
	SOURCEDIR=`pwd` ; \
	echo cd $(@D) ; cd $(@D) ; \
	echo $(YACC) $(YACCFLAGS) $$SOURCEDIR/$(YSOURCE) ; \
	$(YACC) $(YACCFLAGS) $$SOURCEDIR/$(YSOURCE) || exit 1; \
	echo "sed -e 's/yy/$(YYNAME)/g' -e 's/$(YYNAME)wrap/yywrap/g' y.tab.c > $(@F)" ; \
	sed -e 's/\(extern.*void.*\)yyerror/\1blabla1/g' \
	-e 's/\(extern.*int.*\)yylex/\1blabla2/g' -e 's/yy/$(YYNAME)/g' \
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

# macro that expands to compilation command for ANSI-C sources:
DOCC =	$(CC) $(CFLAGS) $(XCFLAGS) -c $(@F:.o=.c) -o $@

# the final dependency for all the ANSI-C sources:
$(C_OBJECTS): $(OBJDIR)/%.o: %.c $(HEADERS)
	$(DOCC)

# main depends on all headers:
$(MAIN): $(OBJDIR)/%.o: %.c $(HEADERS)
	$(DOCC)

##############################################################################
##                                                                          ##
##			 E N D   O F   M A K E F I L E                      ##
##                                                                          ##
##############################################################################
