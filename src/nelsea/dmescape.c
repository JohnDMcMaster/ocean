static char *SccsId = "@(#)dmescape.c 3.52 (TU-Delft) 09/01/99";
/* static char *SccsId = "@(#)dmescape.c 4.15 (TU-Delft) 09/01/98"; */

/* This file is used by several programs, e.g. space and cgi.
/* The SCCS file should be updated ONLY in space/auxil !!!!!!
/* The other SCCS directories contain symbolic links to 
/* space/auxil/SCCS.
*/

# ifdef ANNOTATE

#include <stdio.h>
#include "dmincl.h"

#ifndef GEO_ANNOTATE 

/* We can not include "include/config.h" because this file
   is also used by other programs (dali, cgi etc.).
*/

/*
 * Currently this only works for libddm release 3,
 * e.g. because of the use of DME_FMT
 */

#include "dmescape.h"

struct geo_anno ganno;

dmGetDataEscape (DM_STREAM * dmfile, int fmt)
{
    int     k = 0;
    char buf[100];

    switch (fmt) {
        case GEO_ANNOTATE:
	    k = dmScanf (dmfile, "%d", &ganno.type);
	    if (k != 1) goto readError;
	    switch (ganno.type) {
		case GA_FORMAT: 
		    k = dmScanf (dmfile, "%ld %ld\n",
			&ganno.o.format.fmajor, &ganno.o.format.fminor);
		    if (k != 2) goto readError2;
		    break;

		case GA_LINE: 
		    k = dmScanf (dmfile, "%lg %lg %lg %lg %d\n",
			&ganno.o.line.x1, &ganno.o.line.y1,
			&ganno.o.line.x2, &ganno.o.line.y2,
			&ganno.o.line.mode);
		    if (k != 5) goto readError2;
		    break;

		case GA_TEXT: 
		/* Need ~-quoted strings, because they can contain spaces. */
		    k = dmScanf (dmfile, "%lg %lg %lg %lg ~%[^~]~\n",
			&ganno.o.text.x, &ganno.o.text.y,
			&ganno.o.text.ax, &ganno.o.text.ay,
			 ganno.o.text.text);
		    if (k != 5) goto readError2;
		    break;

		case GA_LABEL: 
		    *ganno.o.Label.Attributes = '\0';
		    k = dmScanf (dmfile, "%s %s %d %lg %lg %lg %lg %s\n",
			ganno.o.Label.name,
			ganno.o.Label.Class,
			&ganno.o.Label.maskno,
			&ganno.o.Label.x, &ganno.o.Label.y,
			&ganno.o.Label.ax, &ganno.o.Label.ay,
			ganno.o.Label.Attributes);
		    if (k != 7 && k != 8) goto readError2;
		    /* handle empty-string special cases
		     */
		    if (*ganno.o.Label.name == '~') *ganno.o.Label.name = '\0';
		    if (*ganno.o.Label.Class == '~') 
                        *ganno.o.Label.Class = '\0';
		    if (*ganno.o.Label.Attributes == '~') 
                        *ganno.o.Label.Attributes = '\0';
		    break;
	    }
	    break;

	default: 
	    dmerrno = DME_FMT;
	    dmError ("dmGetDesignData");
	    return (-1);
    }
    return (1);

readError: 
    if (k == EOF) return (0);

readError2: 
    dmerrno = DME_GET;
    sprintf (buf, "fmt = %d", fmt);
    dmError (buf);
    return (-1);
}


int dmPutDataEscape (DM_STREAM * dmfile, int fmt)
{
    int     k;
    char buf[100];

    switch (fmt) {
	case GEO_ANNOTATE: 
	    k = dmPrintf (dmfile, "%d ", ganno.type);
	    if (k < 0)
		goto writeError;

	    switch (ganno.type) {
		case GA_FORMAT: 
		    k = dmPrintf (dmfile, "%d %d\n",
			ganno.o.format.fmajor, ganno.o.format.fminor);
		    if (k < 0) goto writeError;
		    break;

		case GA_LINE: 
		    k = dmPrintf (dmfile, "%.8lg %.8lg %.8lg %.8lg %d\n",
			ganno.o.line.x1, ganno.o.line.y1,
			ganno.o.line.x2, ganno.o.line.y2,
			ganno.o.line.mode);
		    if (k < 0) goto writeError;
		    break;

		case GA_TEXT: 
		/* Need ~-quoted strings, because they can contain spaces. */
		    k = dmPrintf (dmfile, "%.8lg %.8lg %.8lg %.8lg ~%s~\n",
			ganno.o.text.x, ganno.o.text.y,
			ganno.o.text.ax, ganno.o.text.ay,
			ganno.o.text.text);
		    if (k < 0) goto writeError;
		    break;

		case GA_LABEL: 
		    /* handle empty-string special cases
		     */
		    k = dmPrintf (dmfile, "%s %s %d %.8lg %.8lg %.8lg %.8lg %s\n",
			(*ganno.o.Label.name != '\0'
			    ? ganno.o.Label.name : "~"),
			(*ganno.o.Label.Class != '\0'
			    ? ganno.o.Label.Class : "~"),
			ganno.o.Label.maskno,
			ganno.o.Label.x, ganno.o.Label.y,
			ganno.o.Label.ax, ganno.o.Label.ay,
                        (*ganno.o.Label.Attributes != '\0'
                            ? ganno.o.Label.Attributes : "~"));
		    if (k < 0) goto writeError;
		    break;
	    }
	    break;

	default: 
	    dmerrno = DME_FMT;
	    dmError ("dmPutDesignData");
	    return (-1);
    }
    return (0);

writeError: 
    dmerrno = DME_PUT;
    sprintf (buf, "fmt = %d", fmt);
    dmError (buf);
    return (-1);
}

#endif

# endif /* ANNOTATE */
