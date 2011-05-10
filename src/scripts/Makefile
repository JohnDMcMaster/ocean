#	@(#)Makefile 1.34 09/12/00 Delft University of Technology
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

# name of the scripts. Note that names of the type "prog_designsys" are
# installed as $OCEAN/bin/$MACHINE/$designsys/$prog.
SCRIPTS = \
	buildsrc \
	dist \
	dofunc_nelsis3 \
	icdman_nelsis3 \
	mksls_nelsis3 \
	rmsdflock \
	seatail \
	playout \
        kissis \
	ICELLS_nelsis3 \
	ICELLS_nelsis4 \
	install_unt_nelsis3 \
	getldm_nelsis3 \
	thearch \
	oceanSet \
	oceanInstall \
	INSTALLATION \
	COMPILATION

# Each tuplet in the LINKNAMES is a pair of files that have
# to be linked during installation:
LINKNAMES = ICELLS_nelsis3 mkopr_nelsis3 \
	    ICELLS_nelsis3 mkepr_nelsis3 \
	    ICELLS_nelsis3 mkvopr_nelsis3 \
	    ICELLS_nelsis3 mkopr1_93_nelsis3 \
	    ICELLS_nelsis3 tutorial_nelsis3 \
	    ICELLS_nelsis3 tutorial1_93_nelsis3 \
	    ICELLS_nelsis4 mkopr_nelsis4 \
	    ICELLS_nelsis4 tutorial_nelsis4 \
	    icdman_nelsis3 ocman_nelsis3

# names of the manual pages
MANPAGE = dofunc.1 fish.1 mksls.1

# object that contains the date of compilation:
THEDATE = $(OBJDIR)/thedate.o

# the root directory of the ocean tree:
OCEAN = /usr1/ocean

# directory where the ocean manual pages live:
SEAMANDIR = $(OCEAN)/man

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

# linker options to include all required libraries:
OLIBS = 

# eXtra flags for the ANSI-C compiler:
XCFLAGS= -O

# the K&R C compiler:
CC = cc

# options for the ANSI-C compiler:
CFLAGS = -DX11 -DCACDCMAP -I$(SEAINCLDIR) -I$(ICDINCLDIR) \
	 -DNELSIS_REL$(NELSIS_RELEASE) -DIMAGE

# the C++ compiler:
CXX = CC

# eXtra flags for C++ compilation:
XCXXFLAGS =

# options for the C++ compiler:
CXXFLAGS = $(CFLAGS)

# the linker:
LD = CC        # Since we have C++ objects, the linker must also be C++

# options for the linker (does NOT include de -l options in OLIBS):
LDFLAGS = -L$(SEALIBDIR) -L$(ICDLIBDIR) $(XLDFLAGS)

# eXtra load flags:
XLDFLAGS = 

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
	"CXXFLAGS=$(XCFLAGS)" \
	"XCXXFLAGS=$(XCFLAGS)" \
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
	      $(INSTALLDIR)/bin

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
program: $(SCRIPTS)
	chmod +rx $(SCRIPTS) 2>/dev/null || \
	(for script in $(SCRIPTS); do \
	     rm -f TMP.$$script ; \
	     cp $$script TMP.$$script && rm -f $$script && \
	     mv TMP.$$script $$script && chmod ugo+rx $$script ; \
	 done)

# install the program in the INSTALLDIR:
install: program $(INSTALLDIRS)
	@( \
	  for SCRIPT in $(SCRIPTS); do \
	     DESIGNSYSTEM=`echo $$SCRIPT | sed -n '/.*_\(.*\)/s//\1/p'` ; \
	     if [ "$$DESIGNSYSTEM" != "" ] ; then \
	        BIN=$(INSTALLDIR)/bin/$(MACHINE)/$$DESIGNSYSTEM ; \
		PROGRAM=`echo $$SCRIPT | sed -n '/\(.*\)_.*/s//\1/p'` ; \
	     else \
	        BIN=$(INSTALLDIR)/bin ; \
		PROGRAM=$$SCRIPT ; \
	     fi ; \
	     mkdir -p $$BIN; \
	     if [ -f $$BIN/$$PROGRAM ] ; then \
	        echo mv -f $$BIN/$$PROGRAM $$BIN/$$PROGRAM.old ; \
	        mv -f $$BIN/$$PROGRAM $$BIN/$$PROGRAM.old ; \
	     fi ; \
	     echo cp $$SCRIPT $$BIN/$$PROGRAM ; \
	     cp $$SCRIPT $$BIN/$$PROGRAM ; \
	     chmod ug+rwx,o+rx $$BIN/$$PROGRAM ; \
	  done ; \
	  set $(LINKNAMES) ; \
	  while [ $$# -ge 2 ]; do \
	     DESIGNSYSTEM1=`echo $$1 | sed -n '/.*_\(.*\)/s//\1/p'` ; \
	     if [ "$$DESIGNSYSTEM1" != "" ] ; then \
	        BIN1=$(INSTALLDIR)/bin/$(MACHINE)/$$DESIGNSYSTEM1 ; \
		PROGRAM1=`echo $$1 | sed -n '/\(.*\)_.*/s//\1/p'` ; \
	     else \
	        BIN1=$(INSTALLDIR)/bin ; \
		PROGRAM1=$$1 ; \
	     fi ; \
	     DESIGNSYSTEM2=`echo $$2 | sed -n '/.*_\(.*\)/s//\1/p'` ; \
	     if [ "$$DESIGNSYSTEM2" != "" ] ; then \
	        BIN2=$(INSTALLDIR)/bin/$(MACHINE)/$$DESIGNSYSTEM2 ; \
		PROGRAM2=`echo $$2 | sed -n '/\(.*\)_.*/s//\1/p'` ; \
	     else \
	        BIN2=$(INSTALLDIR)/bin ; \
		PROGRAM2=$$1 ; \
	     fi ; \
	     shift; shift; \
	     mkdir -p $$BIN1 $$BIN2; \
	     echo ln $$BIN1/$$PROGRAM1 $$BIN2/$$PROGRAM2 ; \
	     rm -f $$BIN2/$$PROGRAM2 ; \
	     ln $$BIN1/$$PROGRAM1 $$BIN2/$$PROGRAM2 ; \
	  done ; \
          echo cp $(MANPAGE) $(SEAMANDIR)/man1 ; \
          (cd $(SEAMANDIR)/man1; rm -f $(MANPAGE)); \
          cp $(MANPAGE) $(SEAMANDIR)/man1 ; \
	  exit 0 \
	 )

# go to a scratch directory, check-out the latest SCCS version and install it:
checkout_and_install: $(BUILDDIR)
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
distribution: $(DISTRIBUTIONDIR)
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

$(BUILDDIR) $(DISTRIBUTIONDIR):; mkdir $@

# remove object files and the program:
clean: ;

# remove object files and the program:
veryclean: ;

# remove the program:
targetclean: ;

# remove the distribution scratch directory and its contents:
distclean: ; rm -rf $(DISTRIBUTIONDIR)

# clean up the directory used by the checkout_and_install rule:
buildclean: ; rm -rf $(BUILDDIR)

# blast away all the scripts...
blast: ; rm -f $(SCRIPTS)

##############################################################################
##                                                                          ##
##			 E N D   O F   M A K E F I L E                      ##
##                                                                          ##
##############################################################################
