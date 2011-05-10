/* static char *SccsId = "@(#)animate.c 3.11 (Delft University of Technology) 09/01/99"; */
/**********************************************************

Name/Version      : seadali/3.11

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : HP9000

Author(s)         : Patrick Groeneveld
Creation date     : 01-Jan-1990


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786240

        COPYRIGHT (C) 1987-1989, All rights reserved
**********************************************************/
#include "header.h"
#include <ctype.h>
#include <string.h>

extern Coor xltb, xrtb, ybtb, yttb; /* total bounding box */
extern int  vis_arr[];
extern int  def_arr[];
extern int  NR_lay;
extern int  pict_arr[];
extern int  Draw_dominant;
extern int  Draw_hashed;
extern int  drawing_order[100];
extern DM_PROJECT *dmproject;

char Input_cell_name[DM_MAXNAME + 1];/* name of pre-specified cell */
int  Default_expansion_level;
char *Print_command;

/* PATRICK NEW */
char ImageInstName[DM_MAXNAME + 1];      /* image name which is skipped */
char ViaInstName[DM_MAXNAME + 1];        /* via name which is skipped */
char Weedout_lib[DM_MAXNAME + 1];        /* Libraries which are not displayed during an add_imp */
int  MaxDrawImage;       /* maximum number of images on the screen */
int  ImageMode;      /* TRUE to snap instances to the grid */
int  BackingStore;

int MaskLink[DM_MAXNOMASKS + 1];

#ifdef IMAGE
extern char *ThisImage; /* image name ( not cell name) , Null if no SoG */ 
#endif
/* END PATRICK */

extern int atoi ();
extern int msk_nbr ();

/* * * * * * *
 *
 * body routine which is called from command()
 */
load_rc_files ()
{
char *getenv ();
char *malloc ();
char *dalirc = ".dalirc";
char *seadalirc = ".seadalirc";
char *home = NULL;
char filepath[256];
FILE *rcfile = NULL;
int
   i;

Default_expansion_level = 1;

/* PATRICK NEW: more initalisations */
/*
 * NOTE: these are default settings which are convenient for Sea-of-Gates use
 */
MaxDrawImage = 120;      /* draw at most 120 image cells on the sceen */ 
strcpy(ImageInstName, "IMAGE");   /* INSTANCE name of the image */
strcpy(ViaInstName,"via");        /* INSTANCE name of vias (1st 3 characters) */
strcpy(Weedout_lib,"");           /* no libraries weeded out */
ImageMode = -1;                   /* init imagemode to no set */
BackingStore = TRUE;
/* init random number generator */
#ifdef NO_RAND48
    srand((unsigned int) getpid()); 
#else
    srand48((long) getpid());
#endif

/*
 * disable coupling of masks
 */
for(i = 0; i <= DM_MAXNOMASKS; i++)
   MaskLink[i] = 0;


/*
 * The tracker is default on
 */
switch_tracker( TRUE );

/* END PATRICK */

/*
 * read dalirc
 */
/* first try in current directory */
strcpy(filepath, seadalirc);
rcfile = fopen (filepath, "r");
if (!rcfile)
   { /* then try in home directory */
   if (home = getenv ("HOME")) 
      {
      sprintf (filepath, "%s/%s", home, seadalirc);
      rcfile = fopen (filepath, "r");
      }
   }
if (!rcfile) 
   { /* then try in process directory */
   strcpy(filepath, dmGetMetaDesignData (PROCPATH, dmproject, "seadalirc"));
   rcfile = fopen (filepath, "r");
   }
if (!rcfile) 
   { /* then try a dalirc file */
   strcpy(filepath, dalirc);
   rcfile = fopen (filepath, "r");
   }
if (!rcfile)
   { /* then try in home directory */
   if (home = getenv ("HOME")) 
      {
      sprintf (filepath, "%s/%s", home, dalirc);
      rcfile = fopen (filepath, "r");
      }
   }
if (!rcfile) 
   { /* then try in process directory */
   strcpy(filepath, dmGetMetaDesignData (PROCPATH, dmproject, "dalirc"));
   rcfile = fopen (filepath, "r");
   }


if (rcfile) 
   {
   char message[256];
   sprintf(message,"Seadali: loading rc file '%s'",filepath);
/*    fprintf(stderr,"%s\n", message); */
   ptext (message);
   do_animate (rcfile);
   fclose (rcfile);
   }
else 
   {
   int i;
   ptext ("No dalirc file found.");
   /*
    * PATRICK disable drawing order
    */
   for (i = 0; i != 100; i++) 
      drawing_order[i] = -1;
   }

/*
 * set the backing store
 */
set_backing_store(BackingStore);

#ifdef IMAGE
/*
 * read the sea-of gates image
 * The routine sets ThisImage
 */
read_SoG_image();
ptext ("SEADALI, your window to Sea-of-Gates layout with OCEAN. Delft Univ. of Technology");

/*
 * if image: do instance snap 
 */
if(ImageMode == -1)
   {
   if(ThisImage == NULL)
      ImageMode = FALSE;
   else
      ImageMode = TRUE;
   }
#else
ImageMode = FALSE;
#endif

/*
 * read pre-specified cell, if it exists
 */
if (strlen (Input_cell_name) == 0)
   return;

if( (int) dmGetMetaDesignData (EXISTCELL, dmproject, Input_cell_name, LAYOUT) != 1)
   {
   ptext("Hey!! Your pre-specified input cell does not exists!!!");
   return;
   }

inp_mod (Input_cell_name);
expansion (Default_expansion_level);
pict_all(ERAS_DR);
picture ();
set_titlebar(NULL);
}

static
do_animate (fp)
FILE *fp;
{
    char    token[200],
            label[200],
            name[200],
            scanline[1024],
    *strsave(),
           *pt,
           *fgets ();
    FILE   *outputfile;
    int     order, i, lay, time, found;
    extern int getLineValues ();
    float   xc, yc, width, height, center_x, center_y, fraction;

    Draw_dominant = FALSE;
    Draw_hashed = FALSE;
/*
 * disable drawing order
 */
    for (i = 0; i != 100; i++) drawing_order[i] = -1;
/*
 * open files
 */
#ifdef TO_OUTP_FILE
    sprintf (outfile, "animate.%d", getpid ());
    if (!(outputfile = fopen (outfile, "w"))) {
	PE "ERROR: Failed to open output file 'output.animation'\n");
	fclose (fp);
	return;
    }
#else
    outputfile = NULL;
#endif

    while (fgets (scanline, 1024, fp) != NULL) {/* main line */
    /*
     * skip comment
     */
	if (scanline[0] == '#' || scanline[0] == '\n') continue;
    /*
     * get token and (if possible) first command
     */
	if (sscanf (scanline, "%s %s", token, label) < 1) continue;
	if (!isalpha (token[0])) continue;
    /*
     * parse token
     */
/*
 * label
 */
	if (strncmp (token, "label", 5) == 0) {/* found label: ignore */
/*      ptext("label"); */
	    continue;
	}
/*
 * go to
 */
	if (strncmp (token, "goto", 4) == 0) {
/*      ptext("go to"); */
	    rewind (fp);
	    found = FALSE;
	    while (fgets (scanline, 1024, fp) != NULL) {
		if (scanline[0] == '#' || scanline[0] == '\n' || sscanf (scanline, "%s %s", token, name) != 2)
		    continue;

		if (strcmp (label, name) == 0) {
		/* found label: continue there */
		    found = TRUE;
		    break;
		}
	    }

	    if (found == TRUE) continue;

	    if (outputfile) {
		fprintf (outputfile, "ERROR: goto: cannot find label '%s'\n", label);
		fflush (outputfile);
	    }
	    continue;
	}
/*
 * print text
 */
	if (strncmp (token, "print", 5) == 0) {/* print text */
/*      ptext("text"); */
	/* wind left of token */
	    for (i = 0, pt = scanline; *pt != 't'; pt++, i++)
				/* nothing */
		;
	/* find first non-space */
	    for (pt++; *pt == ' ' || *pt == '\t'; pt++, i++)/* nothing */
		;
	/* terminate string by NULL */
	    for (; i < 200 && isprint (scanline[i]); i++)/* nothing */
		;
	    scanline[i] = '\0';
	/* print */
	    if (outputfile) {
		fprintf (outputfile, "%s\n", pt);
		fflush (outputfile);
	    }
	    ptext (pt);
	    continue;
	}
/*
 * sleep
 */
	if (strncmp (token, "sleep", 5) == 0) {
	/*     ptext("sleeping"); */
	    if (sscanf (scanline, "%s %d", token, &time) != 2) {
		PE "ERROR: sleep: no time\n");
		continue;
	    }
	    sleep ((unsigned long) time);
	    continue;
	}
/*
 * beep
 */
	if (strncmp (token, "beep", 4) == 0) {
	    if (outputfile) {
		fprintf (outputfile, "");
		fflush (outputfile);
	    }
	    continue;
	}
/*
 * read_cell
 */
	if (strncmp (token, "read", 4) == 0) {
/*      ptext("reading cell"); */
	    if (sscanf (scanline, "%s %s", token, name) != 2) {
		PE "ERROR: read: no file\n");
		continue;
	    }
	    inp_mod (name);
	    picture ();
	    continue;
	}
/*
 * redraw
 */
	if (strncmp (token, "redraw", 6) == 0) {
/*      ptext("redrawing"); */
	    picture ();
	    continue;
	}
/*
 * wdw_bbx
 */
	if (strncmp (token, "wdw_bbx", 7) == 0) {
/*      ptext("wdw_bbx"); */
	    bound_w ();
	    picture ();
	    continue;
	}
/*
 * zoom <xl> <yb> <fraction>
 */
	if (strncmp (token, "zoom", 4) == 0) {
/*      ptext("zooming"); */
	    if (sscanf (scanline, "%s %f %f %f", token, &xc, &yc, &fraction) != 4) {
		PE "ERROR: zoom: no range\n");
		continue;
	    }

	    width = (xrtb - xltb) * fraction;
	    height = (yttb - ybtb) * fraction;
	    center_x = xltb + ((xrtb - xltb) * xc);
	    center_y = ybtb + ((yttb - ybtb) * yc);

	    curs_w ((Coor) (center_x - (width / 2)), (Coor) (center_x + (width / 2)),
		    (Coor) (center_y - (height / 2)), (Coor) (center_y + (height / 2)));
	    picture ();
	    continue;
	}
/*
 * center_w <xl> <yb>
 */
	if (strncmp (token, "center", 6) == 0) {
/*      ptext("centering"); */
	    if (sscanf (scanline, "%s %f %f", token, &xc, &yc) != 4) {
		PE "ERROR: center: no range\n");
		continue;
	    }

	    center_x = xltb + ((xrtb - xltb) * xc);
	    center_y = ybtb + ((yttb - ybtb) * yc);
	    center_w ((Coor) center_x, (Coor) center_y);
	    picture ();
	    continue;
	}
/*
 * expand
 */
	if (strncmp (token, "expand", 6) == 0 || strncmp (token, "all_exp", 7) == 0) {
/*      ptext("expanding"); */
	    if (sscanf (scanline, "%s %d", token, &time) != 2) {
		PE "ERROR: exp: no level\n");
		continue;
	    }
	    expansion (time);
	    picture ();
	    continue;
	}
/*
 * set default expansion level
 */
	if (strncmp (token, "default_expan", 13) == 0) {
	    if (sscanf (scanline, "%s %d", token, &time) != 2) {
		PE "ERROR: default_expansion_level: no level\n");
		continue;
	    }
	    time = Max (1, time);
	    time = Min (9, time);
	    Default_expansion_level = time;
	    continue;
	}
/*
 * set visible layer
 */
	if (strncmp (token, "visible", 7) == 0) {
/*      ptext("visible"); */
	    if ((found = sscanf (scanline, "%s %s %s", token, name, label)) < 2) {
		PE "ERROR: visible: no layer name\n");
		continue;
	    }

	    if (strcmp (name, "terminals") == 0)
		lay = NR_lay;
	    else if (strcmp (name, "instances") == 0)
		lay = NR_lay + 2;
	    else if (strcmp (name, "bbox") == 0)
		lay = NR_lay + 3;
	    else if (strcmp (name, "term_name") == 0)
		lay = NR_lay + 4;
	    else if (strcmp (name, "inst_name") == 0)
		lay = NR_lay + 6;
	    else
		lay = msk_nbr (name);
	    if (lay <= -1) {
		PE "Error in command 'visible'\n");
		continue;
	    }

	    if (found > 2 && strcmp (label, "on") == 0) {
		if (vis_arr[lay] == FALSE)
		    pict_arr[lay] = DRAW;
		vis_arr[lay] = TRUE;
		continue;
	    }

	    if (found > 2 && strcmp (label, "off") == 0) {
		if (vis_arr[lay] == TRUE)
		    pict_arr[lay] = ERAS_DR;
		vis_arr[lay] = FALSE;
		continue;
	    }

	    if (found > 2)
		PE "ERROR: visible: value '%s' is not 'on' nor 'off' (toggeling)\n", label);

	/* toggle */
	    if (vis_arr[lay] == FALSE) {
		vis_arr[lay] = TRUE;
		pict_arr[lay] = DRAW;
	    }
	    else {
		vis_arr[lay] = FALSE;
		pict_arr[lay] = ERAS_DR;
	    }

	/* requires redraw command */
	    continue;
	}
/*
 * set layer for drawing
 */
	if (strncmp (token, "layer", 5) == 0) {
/*      ptext("layer"); */
	    if ((found = sscanf (scanline, "%s %s %s", token, name, label)) < 2) {
		PE "ERROR: layer: no layer name\n");
		continue;
	    }

	    lay = msk_nbr (name);

	    if (lay <= -1) {
		PE "Error in command 'layer'\n");
		continue;
	    }

	    if (found > 2 && strcmp (label, "on") == 0) {
		def_arr[lay] = TRUE;
		bulb (lay, TRUE);
		continue;
	    }

	    if (found > 2 && strcmp (label, "off") == 0) {
		def_arr[lay] = FALSE;
		bulb (lay, FALSE);
		continue;
	    }

	    if (found > 2)
		PE "ERROR: layer: value '%s' is not 'on' nor 'off' (toggeling)\n", label);

	/* toggle */
	    if (vis_arr[lay] == FALSE)
		def_arr[lay] = TRUE;
	    else
		def_arr[lay] = FALSE;

	    bulb (lay, def_arr[lay]);
	/* requires redraw command */
	    continue;
	}
/*
 * set drawing order
 */
	if (strncmp (token, "drawing_order", 13) == 0) {
	    static int cur_pos = 0;
	    static int * spec_masks = NULL;
	    int index;
            int noValues = 0;
	    int * mask_order;

	    if (spec_masks == NULL) {
	    /* We do this to make sure that masks are not entered more than
	     * once.
	     */
	        spec_masks = (int *) calloc (NR_lay, sizeof (int));
	    }

	    noValues = getLineValues (scanline, & mask_order, msk_nbr);

	    for (index = 0; index < noValues; index++, cur_pos++) {
		if (mask_order[index] < 0 ||
					spec_masks[mask_order[index]] == 1) {
		    PE "Error in command 'drawing_order'\n");
		    cur_pos--;
		}
		else {
		    drawing_order[cur_pos] = mask_order[index];
		    spec_masks[mask_order[index]] = 1;
		}
	    }
	    continue;
	}

	if (strncmp (token, "order", 5) == 0) {
/*      ptext("order"); */
	    if ((found = sscanf (scanline, "%s %s %d", token, name, &order)) < 3) {
		PE "ERROR: order: no layer name or order\n");
		continue;
	    }

	    lay = msk_nbr (name);

	    if (lay <= -1) {
		PE "Error in command 'order'\n");
		continue;
	    }

	    if (order < 0 || order >= NR_lay) {
		PE "ERROR: order: illegal order number: %d\n", order);
		continue;
	    }
	    if (drawing_order[order] >= 0) {
		PE "WARNING: order position %d multiply declared\n", order);
	    }
	    drawing_order[order] = lay;
	    continue;
	}
/*
 * append <xl> <xr> <yb> <yt>
 */
	if (strncmp (token, "append", 6) == 0) {
	    Coor xl, xr, yb, yt;
	    if (sscanf (scanline, "%s %ld %ld %ld %ld", token, &xl, &xr, &yb, &yt) != 5) {
		PE "ERROR: append: no rectangle\n");
		continue;
	    }
	/* flicker effect */
	    addel_cur (xl, xr, yb, yt, ADD);
	    picture ();
	    addel_cur (xl, xr, yb, yt, DELETE);
	    picture ();
	    addel_cur (xl, xr, yb, yt, ADD);
	    picture ();
	    continue;
	}
/*
 * delete <xl> <xr> <yb> <yt>
 */
	if (strncmp (token, "delete", 6) == 0) {
	    Coor xl, xr, yb, yt;
	    if (sscanf (scanline, "%s %ld %ld %ld %ld", token, &xl, &xr, &yb, &yt) != 5) {
		PE "ERROR: delete: no rectangle\n");
		continue;
	    }
	    addel_cur (xl, xr, yb, yt, DELETE);
	    picture ();
	    continue;
	}
/*
 * draw dominant/non dominant
 */
	if (strncmp (token, "dominant", 8) == 0) {
	    if ((found = sscanf (scanline, "%s %s", token, label)) < 1) {
		PE "ERROR: dominant: error\n");
		continue;
	    }

	    if (found >= 2 && strcmp (label, "off") == 0) {
		Draw_dominant = FALSE;
		continue;
	    }

	    Draw_dominant = TRUE;
	    continue;
	}

/* PATRICK NEW */

/*
 * draw hashed /non hashed instances
 */
	if (strncmp (token, "hashed", 4) == 0) {
	    if ((found = sscanf (scanline, "%s %s", token, label)) < 1) {
		PE "ERROR: hashed: error\n");
		continue;
	    }

	    if (found >= 2 && strcmp (label, "off") == 0) {
		Draw_hashed = FALSE;
		continue;
	    }

	    Draw_hashed = TRUE;
	    continue;
	}

/*
 * snap instances
 */
	if (strncmp (token, "imagemode", 7) == 0) {
	    if ((found = sscanf (scanline, "%s %s", token, label)) < 1) {
		PE "ERROR: ImageMode: error\n");
		continue;
	    }

	    if (found >= 2 && strcmp (label, "off") == 0) {
		ImageMode = FALSE;
		continue;
	    }

	    ImageMode = TRUE;
	    continue;
	}

/*
 * grid on or off
 */
	if (strncmp (token, "grid", 4) == 0) {
	    if ((found = sscanf (scanline, "%s %s", token, label)) < 1) {
		PE "ERROR: grid: error\n");
		continue;
	    }

	    if ((found >= 2 && strcmp (label, "off") == 0)) {
		switch_grid(FALSE);
		continue;
	    }

            switch_grid(TRUE);
	    continue;
	}

/*
 * tracker on or off
 */
	if (strncmp (token, "tracker", 6) == 0) {
	    if ((found = sscanf (scanline, "%s %s", token, label)) < 1) {
		PE "ERROR: tracker: error\n");
		continue;
	    }

	    if ((found >= 2 && strcmp (label, "off") == 0)) {
		switch_tracker(FALSE);
		continue;
	    }

            switch_tracker(TRUE);
	    continue;
	}

/*
 * backing store
 */
	if (strncmp (token, "backingstore", 7) == 0) {
	    if ((found = sscanf (scanline, "%s %s", token, label)) < 1) {
		PE "ERROR: backingstore: error\n");
		continue;
	    }

	    if (found >= 2 && strcmp (label, "off") == 0) {
		BackingStore = FALSE;
		continue;
	    }

	    BackingStore = TRUE;
	    continue;
	}

/*
 * ImageName
 */
	if (strncmp (token, "imagename", 7) == 0) {
	    if ((found = sscanf (scanline, "%s %s", token, name)) != 2) {
		PE "ERROR: image: no image name\n");
		continue;
	    }

            if(strlen(name) > DM_MAXNAME)
               {
               PE "ERROR: image: name too long\n");
               continue;
               }   

            strcpy(ImageInstName, name);
            continue;

            }

/*
 * ViaName
 */
	if (strncmp (token, "via", 5) == 0) {
	    if ((found = sscanf (scanline, "%s %s", token, name)) != 2) {
		PE "ERROR: via: no name\n");
		continue;
	    }

            if(strlen(name) > DM_MAXNAME)
               {
               PE "ERROR: via: name too long\n");
               continue;
               }   

            strcpy(ViaInstName, name);
            continue;

            }

/*
 * Weedout_lib
 */
	if (strncmp (token, "weed", 4) == 0) {
	    if ((found = sscanf (scanline, "%s %s", token, name)) != 2) {
		PE "ERROR: weedout: no name\n");
		continue;
	    }

            if(strlen(name) > DM_MAXNAME)
               {
               PE "ERROR: weedout: name too long\n");
               continue;
               }   

            if(strlen(Weedout_lib) > 0)
               {
               fprintf(stderr,"Warning: Weedout library '%s' overwritten by '%s'\n",
		       Weedout_lib, name);
	       }

            strcpy(Weedout_lib, name);
            continue;

            }

/* 
 * MAXDRAW
 */
	if (strncmp (token, "maxdraw", 5) == 0) {
	    if ((found = sscanf (scanline, "%s %d", token, &MaxDrawImage)) != 2)                { 
		PE "ERROR: maxdraw: no value\n");
		continue;
	       }
            continue;
            }


	if (strncmp (token, "linkmasks", 8) == 0) {
            int noValues = 0;
	    int * mask_values;

	    noValues = getLineValues (scanline, & mask_values, msk_nbr);

	    link_masks (noValues, mask_values);
	    continue;
	}

/*
 * Printing
 */
        if (strncmp (token, "hardcopy_command", 8) == 0) 
           { /* it's the print command */
           Print_command = strsave(&scanline[17]);
/*	   fprintf(stderr,"Print command: '%s'\n", Print_command); */
	   continue;
	   }
/* END PATRICK */
            

	if (strncmp (token, "wire_width_values", 17) == 0) {
	    int noValues = 0;
	    int * wire_values;

	    noValues = getLineValues (scanline, & wire_values, atoi);

	    set_wire_values (noValues, wire_values);

	    continue;
	}

	if (strncmp (token, "wire_width", 10) == 0) {
	    int wire_width;

	    wire_width = atoi (label);
	    set_wire_width (wire_width);

	    continue;
	}

	if (strncmp (token, "wire_extension", 14) == 0) {
	    if ((found = sscanf (scanline, "%s %s", token, label)) < 1) {
		PE "ERROR: wire_extension\n");
		continue;
	    }

	    if (found > 1) {
		if (strcmp (label, "on") == 0) {
		    set_wire_ext (TRUE);
		    continue;
		}
		if (strcmp (label, "off") == 0) {
		    set_wire_ext (FALSE);
		    continue;
		}
		PE "ERROR: wire_extension: wrong value '%s'\n", label);
	    }
	    else {
		set_wire_ext (TRUE);
	    }
	    continue;
	}

	if (strncmp (token, "no_grid_adjust", 14) == 0) {
	    no_grid_adjust ();
	    continue;
	}

	if (strncmp (token, "display_grid_width", 18) == 0) {
	    int grid_width;

	    grid_width = atoi (label);
	    no_grid_adjust ();
	    set_grid_width (grid_width);

	    continue;
	}

	if (strncmp (token, "display_grid_values", 19) == 0) {
            int noValues = 0;
	    int * grid_values;

	    noValues = getLineValues (scanline, & grid_values, atoi);

	    set_grid_values (noValues, grid_values);
	    continue;
	}

	if (strncmp (token, "snap_grid_width", 15) == 0) {
	    int snap_grid_width;

	    snap_grid_width = atoi (label);
	    set_snap_grid_width (snap_grid_width);

	    continue;
	}

	if (strncmp (token, "snap_grid_values", 16) == 0) {
            int noValues = 0;
	    int * grid_values;

	    noValues = getLineValues (scanline, & grid_values, atoi);

	    set_sn_grid_values (noValues, grid_values);
	    continue;
	}

	if (strncmp (token, "snap_grid_offset", 16) == 0) {
	    int x_offset;
	    int y_offset;

	    sscanf (scanline, "%*s %d %d", &x_offset, &y_offset);
	    set_sn_grid_offset (x_offset, y_offset);
	    continue;
	}

	if (strncmp (token, "flat_expansion", 14) == 0) {
	    if ((found = sscanf (scanline, "%s %s", token, label)) < 1) {
		PE "ERROR: flat_expansion\n");
		continue;
	    }

	    if (found > 1) {
		if (strcmp (label, "on") == 0) {
		    set_flat_expansion (TRUE);
		    continue;
		}
		if (strcmp (label, "off") == 0) {
		    set_flat_expansion (FALSE);
		    continue;
		}
		PE "ERROR: flat_expansion: wrong value '%s'\n", label);
	    }
	    else {
		set_flat_expansion (TRUE);
	    }
	    continue;
	}

	if (strncmp (token, "disabled_layers", 15) == 0) {
            int noValues = 0;
	    int * mask_values;

	    noValues = getLineValues (scanline, & mask_values, msk_nbr);

	    disable_masks (noValues, mask_values);
	    continue;
	    }

	PE "WARNING: token not recognized: '%s'\n", token);
    }

    if (outputfile) fclose (outputfile);
/*     ptext ("Ready!"); */
}

int
getLineValues (line, values, theFunc)
char * line;
int ** values;
int (* theFunc) ();
{
    int maxValues = 10;
    int i;
    extern char * strtok ();
    char * seperators = " \t\n\0";

    * values = (int *) calloc (maxValues, sizeof (int));

    /* get the (integer, base 10) values from line and put them in values */

    /* let's try strtok () */

    strtok (line, seperators); /* the token must be skipped first */

    for (i = 0; ; i++) {
	char * aValue;
	aValue = strtok (NULL, seperators); /* curr. value */
	if (aValue == NULL) {
	    break;
	}
	if (i >= maxValues) {
	    maxValues += 10;
	    * values = (int *) realloc (* values, maxValues * sizeof (int));
	}

	(* values)[i] = (* theFunc) (aValue);
    }
    return i;
}
