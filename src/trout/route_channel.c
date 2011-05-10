/* SccsId = "@(#)route_channel.c 3.1 (TU-Delft) 12/05/91"; */
/**********************************************************

Name/Version      : searoute/3.1

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld
Creation date     : december 1991
Modified by       : 
Modification date :


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240
	e-mail: patrick@donau.et.tudelft.nl

        COPYRIGHT (C) 1991 , All rights reserved
**********************************************************/
/*
 *
 *  Route_channel.c
 *
 * 
 *************************************************/



/*
 * this routine performs the routing of a single channel
 */
route_channel(chan)
RCHANPTR
   chan;
{

/*
 *


