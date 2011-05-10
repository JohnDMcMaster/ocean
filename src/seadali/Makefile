#	@(#)Makefile 3.22 09/01/99 Delft University of Technology
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
PROGRAM = $(BINDIR)/seadali

# the local header files:
HEADERS = \
	comment.h \
	header.h \
	tversion.h 

# objects for ANSI-C compilation. For each object $(OBJDIR)/*.o there
# must exist a corresponding *.c file in the current directory:
C_OBJECTS = \
	$(OBJDIR)/add_poly.o \
	$(OBJDIR)/add_quad.o \
	$(OBJDIR)/add_wire.o \
	$(OBJDIR)/animate.o \
	$(OBJDIR)/array.o \
	$(OBJDIR)/bintree.o \
	$(OBJDIR)/bound.o \
	$(OBJDIR)/buffer.o \
	$(OBJDIR)/check.o \
	$(OBJDIR)/clip.o \
	$(OBJDIR)/command.o \
	$(OBJDIR)/comment.o \
	$(OBJDIR)/del_quad.o \
	$(OBJDIR)/disp_mask.o \
	$(OBJDIR)/display.o \
	$(OBJDIR)/distance.o \
	$(OBJDIR)/dsgn_dat.o \
	$(OBJDIR)/edge_t_ch.o \
	$(OBJDIR)/expand.o \
	$(OBJDIR)/ext_check.o \
	$(OBJDIR)/fish.o \
	$(OBJDIR)/get.o \
	$(OBJDIR)/global.o \
	$(OBJDIR)/grid.o \
	$(OBJDIR)/inform.o \
	$(OBJDIR)/input.o \
	$(OBJDIR)/insert.o \
	$(OBJDIR)/inst_draw.o \
	$(OBJDIR)/mask.o \
	$(OBJDIR)/memo.o \
	$(OBJDIR)/menu.o \
	$(OBJDIR)/mod.o \
	$(OBJDIR)/modl.o \
	$(OBJDIR)/mpar.o \
	$(OBJDIR)/output.o \
	$(OBJDIR)/picture.o \
	$(OBJDIR)/poly_trap.o \
	$(OBJDIR)/precheck.o \
	$(OBJDIR)/q_build.o \
	$(OBJDIR)/q_clear.o \
	$(OBJDIR)/q_delete.o \
	$(OBJDIR)/q_insert.o \
	$(OBJDIR)/q_search.o \
	$(OBJDIR)/real_main.o \
	$(OBJDIR)/s_o_g.o \
	$(OBJDIR)/screen.o \
	$(OBJDIR)/sngrid.o \
	$(OBJDIR)/subr.o \
	$(OBJDIR)/term.o \
	$(OBJDIR)/termbuf.o \
	$(OBJDIR)/tlift.o \
	$(OBJDIR)/tpar.o \
	$(OBJDIR)/window.o \
	$(OBJDIR)/wire.o \
	$(OBJDIR)/dmescape.o

# objects for C++ compilation. For each object $(OBJDIR)/*.o there
# must exist a corresponding *.C file in the current directory:
CXX_OBJECTS = \
	$(OBJDIR)/main.o

# object that contains the date of compilation:
THEDATE = $(OBJDIR)/thedate.o

# the root directory of the ocean tree:
OCEAN = /usr1/ocean

# directory where the ocean header files live:
SEAINCLDIR = $(OCEAN)/include

# directory where the ocean libraries live:
SEALIBDIR = $(OCEAN)/lib/${MACHINE}

# linker options to include ocean libraries (order IS important):
SEALIBS = -lnelsea$(NELSIS_RELEASE) -locean -lseadif

# for which nelsis release are we compiling?
NELSIS_RELEASE = 3

# the root directory of the nelsis tree:
ICDPATH = /usr/cacd

# directory where the nelsis header files live:
ICDINCLDIR = $(ICDPATH)/lib/include

# directory where the nelsis libraries live:
ICDLIBDIR = $(ICDPATH)/lib/$(MACHINE)

# linker options to include nelsis libraries:
ICDLIBS = -llayfmt -lcirfmt -lddm

# linker options to include the X11 stuff:
XLIBS = -lcmap -lX11

# eXtra OLIBS:
XOLIBS = -ll -lm

# linker options to include all required libraries:
OLIBS = $(SEALIBS) $(ICDLIBS) $(XLIBS) $(XOLIBS)

# the K&R C compiler:
CC = cc

# eXtra flags for the ANSI-C compiler:
XCFLAGS= -O

# options for the ANSI-C compiler:
CFLAGS = -DX11 -DCACDCMAP -I$(SEAINCLDIR) -I$(ICDINCLDIR) \
	 -DNELSIS_REL$(NELSIS_RELEASE) -DIMAGE -DANNOTATE

# the C++ compiler:
CXX = CC

# eXtra flags for C++ compilation:
XCXXFLAGS = $(XCFLAGS)

# options for the C++ compiler:
CXXFLAGS = $(CFLAGS) $(XCXXFLAGS)

# the linker:
LD = $(CXX)        # Since we have C++ objects, the linker must also be C++

# eXtra load flags:
XLDFLAGS = $(XCFLAGS)

# options for the linker (does NOT include de -l options in OLIBS):
LDFLAGS = $(XLDFLAGS) -L$(SEALIBDIR) -L$(ICDLIBDIR) 

# directory to install the PROGRAM:
INSTALLDIR = $(OCEAN)

# some targets (noticeably checkout_and_install) may call an inferior "make".
# This defines which macros will be inherited by the inferior make process:
INHERIT_MACROS = \
	"OCEAN=$(OCEAN)" \
	"ICDPATH=$(ICDPATH)" \
	"NELSIS_RELEASE=$(NELSIS_RELEASE)" \
	"INSTALLDIR=$(INSTALLDIR)" \
	"CFLAGS=$(CFLAGS)" \
	"XCFLAGS=$(XCFLAGS)" \
	"CXXFLAGS=$(CXXFLAGS)" \
	"XCXXFLAGS=$(XCXXFLAGS)" \
	"LDFLAGS=$(LDFLAGS)" \
	"XLDFLAGS=$(XLDFLAGS)" \
	"LD=$(LD)" \
	"CC=$(CC)" \
	"CXX=$(CXX)"

##############################################################################
##                                                                          ##
##			     S T A T I C   A R E A                          ##
##                                                                          ##
##############################################################################

# the shell to exec the command lines in this Makefile. Should be /bin/sh:
SHELL = /bin/sh

# subdirectories to contain the objects and the executable:
OBJDIR = objects/$(MACHINE)/nelsis$(NELSIS_RELEASE)
BINDIR = bin/$(MACHINE)/nelsis$(NELSIS_RELEASE)

# a scratch directory to check-out, build and install the latest version:
BUILDDIR = TMP.install

# a scratch dir to check-out the latest version and build a tar archive:
DISTRIBUTIONDIR = TMP.distrib

# All subdirectories that must exist before attempting compilation. In
# this list parent directories must appear before descendant directories:
SUBDIRS = objects objects/$(MACHINE) $(OBJDIR) bin bin/$(MACHINE) $(BINDIR)

# All subdirectories of the INSTALLDIR that must exist before attempting
# installation. In this list parent directories must appear before descendant
# directories:
INSTALLDIRS = $(INSTALLDIR) \
	      $(INSTALLDIR)/bin \
	      $(INSTALLDIR)/bin/$(MACHINE) \
	      $(INSTALLDIR)/$(BINDIR)

# all the objects zusammen:
OBJECTS = $(C_OBJECTS) $(CXX_OBJECTS) $(THEDATE)

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
	      $(PROGRAM) \
	      core

# remove object files and the program:
veryclean: checkmachine
	rm -f $(OBJECTS) \
	      $(PROGRAM) \
	      core

# remove the program:
targetclean: checkmachine
	rm -f $(PROGRAM)

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
	echo "rm -f $$CXX_SOURCES $$C_SOURCES [mM]akefile thedate.[cC] $(HEADERS)" ; \
	rm -f $$CXX_SOURCES $$C_SOURCES [mM]akefile thedate.[cC] $(HEADERS) ; \
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
