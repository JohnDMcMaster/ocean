/* ***************************************************************************
 *  TMIO - extensions to standard seadif database operations supporting      *
 *         delay modeling of circuits.                                       *
 *         This file should be included inside sealibio.h                       *
 *                                                                           *
 *  Author : Ireneusz Karkowski 1992                                         *
 *  SccsId = @(#)tmio.h 1.2 Delft University of Technology 04/29/94 
 *****************************************************************************/

#ifndef  __TMIO_H
#define  __TMIO_H

				  /* Declare that now a circuit has one field more */

#define SDFCIRTM       0x00008000L 
#undef  SDFCIRALL
#define SDFCIRALL      0x0000ff00L

				  /* bacause of the lack of bits left the only 
				     possibility is always to read all timing 
                                     structures for the circuit */


#endif




