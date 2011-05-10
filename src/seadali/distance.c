/* static char *SccsId = "@(#)distance.c 3.1 (Delft University of Technology) 08/14/92"; */
/**********************************************************

Name/Version      : seadali/3.1

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : HP9000

Author(s)         : P. van der Wolf
Creation date     : 18-Dec-1984


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1987-1988, All rights reserved
**********************************************************/
#include "header.h"

/*
** Find minimum distance between two line pieces.
** INPUT: pointers to the edges.
** OUTPUT: the minimum distance between the edges.
*/
distance (p, q, sh_d)
struct f_edge *p, *q, *sh_d;
{
    struct f_edge s1;
    int result1, result2;

    result1 = do_distance (p, q, sh_d);

    s1.xs = sh_d -> xs;
    s1.ys = sh_d -> ys;
    s1.xe = sh_d -> xe;
    s1.ye = sh_d -> ye;

    result2 = do_distance (q, p, sh_d);

    if (result1 < result2) {

	sh_d -> xs = s1.xs;
	sh_d -> ys = s1.ys;
	sh_d -> xe = s1.xe;
	sh_d -> ye = s1.ye;
	return (result1);
    }
    return (result2);
}

/*
** Calculate minimum distance from start and
** stop coordinats of p to edge q.
** INPUT: pointers to the edges p and q.
*/
static
do_distance (p, q, short_d)
struct f_edge *p, *q, *short_d;
{
    float dis ();
    float crossy;
    float crossx;
    float d1, d2;
    float tmp1, tmp2;
    short legal;

    /*
    ** Find distance from ps to line q.
    */
    switch (q -> dir) {
    case -1:
	crossy = p -> ys - (p -> xs - q -> xs - q -> ys + p -> ys) / 2;
	break;
    case 0:
	crossy = p -> ys;
	break;
    case 1:
	crossy = p -> ys + (p -> xs - q -> xs + q -> ys - p -> ys) / 2;
	break;
    case 2:
	crossy = q -> ys;
	break;
    default:
	break;
    }

    /*
    ** Check if intersection crossy on line q.
    */
    legal = TRUE;
    if ((crossy < Min (q -> ys, q -> ye)) || (crossy > Max (q -> ys, q -> ye)))
	legal = FALSE;
    else
	if ((q -> dir == 2) && ((p -> xs < Min (q -> xs, q -> xe)) || (p -> xs > Max (q -> xs, q -> xe))))
	    legal = FALSE;


    if (!(legal)) {
	tmp1 = dis (p -> xs, p -> ys, q -> xs, q -> ys);
	tmp2 = dis (p -> xs, p -> ys, q -> xe, q -> ye);
	d1 = Min (tmp1, tmp2);

	short_d -> xs = p -> xs;
	short_d -> ys = p -> ys;

	if (tmp1 < tmp2) {
	    short_d -> xe = q -> xs;
	    short_d -> ye = q -> ys;
	}
	else {
	    short_d -> xe = q -> xe;
	    short_d -> ye = q -> ye;
	}
    }
    else {
	switch (q -> dir) {
	case -1:
	    crossx = q -> xs - crossy + q -> ys;
	    break;
	case 0:
	    crossx = q -> xs;
	    break;
	case 1:
	    crossx = q -> xs + crossy - q -> ys;
	    break;
	case 2:
	    crossx = p -> xs;
	    break;
	default:
	    break;
	}
	d1 = dis (p -> xs, p -> ys, crossx, crossy);

	short_d -> xs = p -> xs;
	short_d -> ys = p -> ys;
	short_d -> xe = crossx;
	short_d -> ye = crossy;

    }

    /*
    ** Find distance from pe to line q.
    */
    switch (q -> dir) {
    case -1:
	crossy = p -> ye - (p -> xe - q -> xs - q -> ys + p -> ye) / 2;
	break;
    case 0:
	crossy = p -> ye;
	break;
    case 1:
	crossy = p -> ye + (p -> xe - q -> xs + q -> ys - p -> ye) / 2;
	break;
    case 2:
	crossy = q -> ys;
	break;
    default:
	break;
    }

    /*
    ** Check if intersection crossy on line q.
    */
    legal = TRUE;
    if ((crossy < Min (q -> ys, q -> ye)) || (crossy > Max (q -> ys, q -> ye)))
	legal = FALSE;
    else
	if ((q -> dir == 2) && ((p -> xe < Min (q -> xs, q -> xe)) || (p -> xe > Max (q -> xs, q -> xe))))
	    legal = FALSE;

    if (!(legal)) {
	tmp1 = dis (p -> xe, p -> ye, q -> xs, q -> ys);
	tmp2 = dis (p -> xe, p -> ye, q -> xe, q -> ye);
	d2 = Min (tmp1, tmp2);

	if (d2 < d1) {
	    short_d -> xs = p -> xe;
	    short_d -> ys = p -> ye;

	    if (tmp1 < tmp2) {
		short_d -> xe = q -> xs;
		short_d -> ye = q -> ys;
	    }
	    else {
		short_d -> xe = q -> xe;
		short_d -> ye = q -> ye;
	    }
	}
    }
    else {
	switch (q -> dir) {
	case -1:
	    crossx = q -> xs - crossy + q -> ys;
	    break;
	case 0:
	    crossx = q -> xs;
	    break;
	case 1:
	    crossx = q -> xs + crossy - q -> ys;
	    break;
	case 2:
	    crossx = p -> xe;
	    break;
	default:
	    break;
	}
	d2 = dis (p -> xe, p -> ye, crossx, crossy);

	if (d2 < d1) {
	    short_d -> xs = p -> xe;
	    short_d -> ys = p -> ye;
	    short_d -> xe = crossx;
	    short_d -> ye = crossy;
	}
    }
    return ((int) (Min (d1, d2) + 0.01));
}

/*
** Calculate distance between two points.
** INPUT: the coordinats of the points.
** OUTPUT: the distance between the points.
*/
float
dis (x1, y1, x2, y2)
float x1, y1, x2, y2;
{
    double  sqrt ();

    return (sqrt ((y2 - y1) * (y2 - y1) + (x2 - x1) * (x2 - x1)));
}
