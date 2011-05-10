/*
 *	@(#)libiofun.h 1.4 Delft University of Technology 12/29/92
 */

#ifndef __LIBIOFUN__
#define __LIBIOFUN__

#ifndef __CPLUSPLUS__
   STRING     cs();
   int        fs();
   void       sdfexit();
   int	      sdfreadcir();
   int        sdflistlay();
   int        sdflistcir();
   int        sdflistfun();
   int        sdflistlib();
   extern CIRCUITPTR thiscir;
#else
extern "C" 
{
   STRING     cs(STRING);
   int	      fs(STRING);
   int	      sdfopen(),sdfclose();
   void	      sdfexit(int);
   int	      sdfreadcir(int, STRING, STRING, STRING);
   int        sdflistlay(int, CIRCUITPTR);
   int        sdflistcir(int, FUNCTIONPTR);
   int        sdflistfun(int, LIBRARYPTR);
   int        sdflistlib(int);
   extern CIRCUITPTR thiscir;
}
#endif

#endif /* __LIBIOFUN__ */
