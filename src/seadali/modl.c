#ifndef lint
/* static char *SccsId = "@(#)modl.c 3.6 (Delft University of Technology) 05/05/94"; */
#endif
/**********************************************************

Name/Version      : seadali/3.6

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
#include <ctype.h>

extern  DM_PROJECT * dmproject;
extern int  new_cmd;

#ifdef IMAGE
extern int ImageMode;
extern char Weedout_lib[];
#endif

struct modlist {
    char  **mname;
    int     nr_mod;
};

int     g_ask_nr;
char   *g_ask_str[LIST_LENGTH + 4];

static struct modlist   mlst;
static struct modlist   alias_lst;

read_modlist ()
{
    void sort_mlst ();
    static int firsttime = TRUE; /* needed for re-read */
    register char **pmod;

    if (firsttime == TRUE) {
	MALLOCN (mlst.mname, char *, 50);
	if (!mlst.mname) {
	    fatal (0001, "read_modlist");
	    return (-1);
	}
	firsttime = FALSE;
    }

    pmod = (char **) dmGetMetaDesignData (CELLLIST, dmproject, LAYOUT);
    if (!pmod) return (-1);
    mlst.nr_mod = 0;
    while (*pmod) {
	if (!add_mlst (LOCAL, *pmod++)) {
	    fatal (0002, "read_modlist");
	    return (-1);
	}
    }
    sort_mlst (LOCAL);
    return (0);
}

read_impclist ()
{
    void sort_mlst ();
    static int firsttime = TRUE; /* needed for re-read */
    register IMPCELL **pmod;

    if (firsttime == TRUE) {
	MALLOCN (alias_lst.mname, char *, 50);
	if (!alias_lst.mname) {
	    fatal (0001, "read_impclist");
	    return (-1);
	}
	firsttime = FALSE;
    }

    pmod = (IMPCELL **) dmGetMetaDesignData (IMPORTEDCELLLIST,
	    dmproject, LAYOUT);
    if (!pmod) return (-1);
    alias_lst.nr_mod = 0;
    for( /* nothing */ ; *pmod != NULL; ++pmod) {
#ifdef IMAGE
/*
 * Patrick: skip names from primitives library
 */
        if(ImageMode == TRUE &&
	   strlen(Weedout_lib) > 0 &&
	   (*pmod)->dmpath != NULL && 
	   strrchr((*pmod)->dmpath,'/') != NULL)
	   {
	   if(strcmp((char * )(strrchr((*pmod)->dmpath,'/') + 1), Weedout_lib) == 0)
	      continue;
	   }
#endif
	if (!add_mlst (IMPORTED, (*pmod) -> alias)) {
	    fatal (0002, "read_impclist");
	    return (-1);
	}
/*	++pmod; */
    }
    sort_mlst (IMPORTED);
    return (0);
}

static int
add_mlst (local_imported, m_name)
int     local_imported;
char   *m_name;
{
    struct modlist *lst_p;
    char   *realloc ();

    lst_p = (local_imported == LOCAL) ? &mlst : &alias_lst;

    if (lst_p -> nr_mod % 50 == 0 && lst_p -> nr_mod > 0) {
	if ((lst_p -> mname = (char **) realloc ((char *) lst_p -> mname,
			(lst_p -> nr_mod + 50) * sizeof (char *))) == NULL) {
	    ptext ("No memory available!");
	    return (FALSE);
	}
    }
    lst_p -> mname[lst_p -> nr_mod] = m_name;
    lst_p -> nr_mod++;
    return (TRUE);
}

void
sort_mlst (local_imported)
int     local_imported;
{
    struct modlist *lst_p;
    int     compare ();
    void    qsort ();

    lst_p = (local_imported == LOCAL) ? &mlst : &alias_lst;

    (void) qsort ((char *) lst_p -> mname,
	    (unsigned) lst_p -> nr_mod, sizeof (char *), compare);
    return;
}

compare (s1, s2)
char **s1, **s2;
{
    return (strcmp (*s1, *s2));
}

char *
ask_cell (local_imported)
int     local_imported;
{
    static int  nl, ni, pnl, pni;
    char    err_str[MAXCHAR];
    char    tmp_str[DM_MAXNAME+1];
    char  **c_list;
    char   *c_name = NULL;
    register int i, j, k, nr_cells;
    int     f, g, i_old;

    if (local_imported == LOCAL) {
	nr_cells = mlst.nr_mod;
	c_list = mlst.mname;
	i = nl; i_old = pnl;
    }
    else {
	nr_cells = alias_lst.nr_mod;
	c_list = alias_lst.mname;
	i = ni; i_old = pni;
    }

    if (nr_cells <= 0) {
	sprintf (err_str, "No %s cell names!",
	    (local_imported == LOCAL ? "local" : "imported"));
	ptext (err_str);
	return (NULL);
    }

    while (i >= nr_cells) i -= LIST_LENGTH;
    while (i_old >= nr_cells) i_old -= LIST_LENGTH;

    f = 0;
    g_ask_str[0] = "-return-";
    if (nr_cells > LIST_LENGTH) {
	g_ask_str[1] = "-keyboard-";
	g_ask_str[2] = "-next-";
	g_ask_str[3] = "-prev-";
	f = 3;
    }

    for (;;) {
ask1:
	if (new_cmd == -1) {
	    if ((k = i + LIST_LENGTH) > nr_cells) k = nr_cells;
	    for (j = i; j < k; ++j)
		g_ask_str[k - j + f] = c_list[j];
	    j = ask (k - i + f + 1, g_ask_str, (int) -1);
	}
	else { /* only by do_add_inst () */
	    j = new_cmd;
	    new_cmd = -1;
	    pre_cmd_proc (j, g_ask_str);
	}

	if (j > f) { /* cellname */
	    c_name = g_ask_str[j];
	    if (dmTestname (c_name) < 0) {
		sprintf (err_str, "Bad cell name '%s'!", c_name);
		ptext (err_str);
		c_name = NULL;
	    }
	    g_ask_nr = j;
	    goto ret;
	}

	if (j <= 0) {
	    ptext ("");
	    goto ret;	/* return */
	}

	if (j == 3) {		/* prev */
	    if ((i -= LIST_LENGTH) < 0) { /* goto end */
		i = 0;
		while (i + LIST_LENGTH < nr_cells) i += LIST_LENGTH;
	    }
	}
	else if (j == 2) {	/* next */
	    if ((i += LIST_LENGTH) >= nr_cells) i = 0; /* begin */
	}
	else {			/* keyboard */
	    j = ask_name ("cellname: ", tmp_str, FALSE);
	    post_cmd_proc (1, g_ask_str);
	    ptext ("");
	    if (j) goto ask1; /* no name, return given! */
	    g = strlen (tmp_str);
	    if (g < 3 && tmp_str[g-1] == '*') { /* set new position */
		if (g == 1) { /* goto previous position! */
		    g = i; i = i_old; i_old = g;
		}
		else {
set_position:
		    j = k = 0;
		    do {
			g = k;
			if ((k += LIST_LENGTH) > nr_cells) break;
			for (; j < k; ++j)
			    if (*tmp_str <= *c_list[j]) {
				k = nr_cells; /* force exit while loop */
				break;
			    }
		    } while (k < nr_cells);
		    if (g != i) { i_old = i; i = g; }
		}
		goto ask1;
	    }
	    else if (dmTestname (tmp_str) < 0) {
		sprintf (err_str, "Bad cell name '%s'!", tmp_str);
		ptext (err_str);
		if (isalpha (*tmp_str)) goto set_position;
		goto ask1;
	    }
	    j = k = 0;
	    do {
		g = k;
		if ((k += LIST_LENGTH) > nr_cells) k = nr_cells;
		for (; j < k; ++j)
		    if (!strcmp (tmp_str, c_list[j])) {
			if (g != i) { i_old = i; i = g; }
			c_name = c_list[j];
			g_ask_nr = -1;
			goto ret;
		    }
	    } while (k < nr_cells);
	    sprintf (err_str, "Unknown cell name '%s'!", tmp_str);
	    ptext (err_str);
	    goto set_position;
	}
    }

ret:
    if (local_imported == LOCAL) {
	nl = i; pnl = i_old;
    }
    else {
	ni = i; pni = i_old;
    }
    return (c_name);
}
