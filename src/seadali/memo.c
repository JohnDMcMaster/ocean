/* static char *SccsId = "@(#)memo.c 3.2 (Delft University of Technology) 08/11/98"; */
/**********************************************************

Name/Version      : seadali/3.2

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

        COPYRIGHT (C) 1987-1989, All rights reserved
**********************************************************/
#include "header.h"

extern DM_CELL *ckey;
extern char *cellstr;
extern qtree_t *quad_root[];
extern INST *inst_root;
extern INST *act_inst;
extern TEMPL *first_templ;	/* templates of instances */
extern TERM *term[];
extern TERM *act_term;
extern int  sub_term_flag;
extern int  exp_level;
extern int  NR_lay;
extern int  checked;

init_mem ()
{
    register int lay;
    qtree_t *qtree_build ();

    ckey = NULL;
    cellstr = NULL;
    inst_root = NULL;
    first_templ = NULL;
    act_inst = NULL;
    act_term = NULL;

    for (lay = 0; lay < NR_lay; ++lay) {
	quad_root[lay] = qtree_build ();
	term[lay] = NULL;
    }
    sub_term_flag = FALSE;
    exp_level = 1;
    checked = FALSE;
}

empty_mem ()
{
    INST *inst_p, *next_inst;
    TEMPL *templ_p, *next_templ;
    TERM *tpntr, *next_term;
    register int lay;

    for (lay = 0; lay < NR_lay; ++lay) {
	quad_clear (quad_root[lay]);
    }

    /* now clear 'root' instances */
    for (inst_p = inst_root; inst_p != NULL; inst_p = next_inst) {
	next_inst = inst_p -> next;
	FREE (inst_p);
    }

    /* now we have a list of 'floating' templates: clear them */
    for (templ_p = first_templ; templ_p != NULL; templ_p = next_templ) {
	clear_templ (templ_p);	/* clears all quads, lower instances, etc. */
	next_templ = templ_p -> next;
	FREE (templ_p);
    }

    for (lay = 0; lay < NR_lay; ++lay) {
	for (tpntr = term[lay]; tpntr != NULL;) {
	    next_term = tpntr -> nxttm;
	    FREE (tpntr);
	    tpntr = next_term;
	}
    }
    empty_err ();

    /* AvG */
#ifdef ANNOTATE
    empty_comments ();
#endif

    init_mem ();
}

clear_templ (templ_p)
TEMPL *templ_p;
{
    TERM *tpntr, *next_term;
    INST *inst_p, *next_inst;
    register int lay;

    for (lay = 0; lay < NR_lay; ++lay) {
	quad_clear (templ_p -> quad_trees[lay]);
	templ_p -> quad_trees[lay] = NULL;

	for (tpntr = templ_p -> term_p[lay]; tpntr != NULL;) {
	    next_term = tpntr -> nxttm;
	    FREE (tpntr);
	    tpntr = next_term;
	}
	templ_p -> term_p[lay] = NULL;
    }
    templ_p -> t_flag = FALSE;

    for (inst_p = templ_p -> inst; inst_p != NULL;) {
	next_inst = inst_p -> next;
	FREE (inst_p);
	inst_p = next_inst;
    }
    templ_p -> inst = NULL;
    templ_p -> projkey = NULL;
}

no_works ()
{
    register int lay;

    for (lay = 0; lay < NR_lay; ++lay)
	if (!quad_is_empty (quad_root[lay])) return (FALSE);
    if (inst_root != NULL) return (FALSE);
    for (lay = 0; lay < NR_lay; ++lay)
	if (term[lay] != NULL) return (FALSE);
#ifdef ESE
    if (!no_comments ()) return (FALSE);
#endif
    return (TRUE);
}

quad_is_empty (Q)
qtree_t *Q;
{
    if (Q == NULL) return (TRUE);
    if (Q -> object) return (FALSE);
    if (Q -> reference) return (FALSE);

    if (!quad_is_empty (Q -> Uleft))  return (FALSE);
    if (!quad_is_empty (Q -> Uright)) return (FALSE);
    if (!quad_is_empty (Q -> Lleft))  return (FALSE);
    if (!quad_is_empty (Q -> Lright)) return (FALSE);
    return (TRUE);
}
