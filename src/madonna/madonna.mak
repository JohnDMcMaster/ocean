.AUTODEPEND

#		*Translator Definitions*
CC = bcc +MADONNA.CFG
TASM = TASM
TLINK = tlink


#		*Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#		*List Macros*


EXE_dependencies =  \
  lia\lia.lib \
  main\main.lib \
  part\part.lib \
  phil\phil.lib \
  ..\sealib\sealib.lib

#		*Explicit Rules*
madonna.exe: madonna.cfg $(EXE_dependencies)
  $(TLINK) /x/i/n/c/d/o /LC:\BC\LIB @&&|
/o- c0l.obj+

		# no map file
lia\lia.lib+
main\main.lib+
/o+ part\part.lib+
phil\phil.lib+
/o- ..\sealib\sealib.lib+
overlay.lib+
fp87.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
#		*Compiler Configuration File*
madonna.cfg: madonna.mak
  copy &&|
-ml!
-a
-f87
-C
-N
-O
-Z
-k-
-rd
-Y
-vi
-H=MADONNA.SYM
-Fc
-wpin
-wamb
-wamp
-wasm
-wpro
-wcln
-wdef
-wsig
-wnod
-wstv
-wucp
-wuse
-IC:\BC\INCLUDE;
-LC:\BC\LIB
-P-.C
-Ff
| madonna.cfg


