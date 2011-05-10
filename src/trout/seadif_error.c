/* SccsId = "@(#)seadif_error.c 3.5 (TU-Delft) 04/29/94"; */
/**********************************************************

Name/Version      : nelsea/3.5

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Patrick Groeneveld
Creation date     : december 1991
Modified by       : 
Modification date : April 15, 1992


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
 *      sdf_error.c
 *
 * maintain seadif error cells
 *
 *********************************************************/
#include  "typedef.h"
#include  "grid.h"
#include  "sealibio.h" 

#define DEFAULT_PROGRAM      "trout"
#define DEFAULT_AUTHOR       "me"
#define DEFAULT_TECHNOLOGY   "default technology"

#define DEFAULT_TYPE         "no type"

extern LAYOUTPTR   
   thislay;
extern CIRCUITPTR
   thiscir;
extern FUNCTIONPTR
   thisfun;
extern LIBRARYPTR
   thislib;

static LAYOUTPTR
   unconnect;      /* pointer to instance which is placed on an unconnect */

/* * * * * * * *
 *
 * This routine initializes the unconnect system
 */
init_unconnect(lay)
LAYOUTPTR
   lay;
{
char
   *unconn;
LAYOUTPTR
   find_sdf_layout_cell();

/*
 * name of unconnected cell
 */
unconn = cs("Error_Marker");
unconnect = NULL;

/*
 * does some unconnect cell exist?
 */
if(sdfaliastoseadif(unconn, ALIASLAY) == TRUE)
   {
   if(existslay(thislaynam, thiscirnam, thisfunnam, thislibnam) == TRUE &&
      sdfreadlay(SDFLAYSTAT, thislaynam, thiscirnam, thisfunnam, thislibnam) == TRUE)
      {
      unconnect = thislay;
      return;
      }
   }

/* add underscore to make different */
unconn = cs("Error_Marker_");

/* does not exists: make it */
unconnect = find_sdf_layout_cell((int) SDFLAYBODY,
				 unconn, unconn, 
				 unconn, lay->circuit->function->library->name);
/*
 * fill unconnect
 */
unconnect->off[X] = unconnect->off[Y] = 0;
unconnect->bbx[X] = unconnect->bbx[Y] = 0;
NewWire(unconnect->wire); 
unconnect->wire->crd[L] = unconnect->wire->crd[L] = 0;
unconnect->wire->crd[B] = unconnect->wire->crd[T] = 0;
unconnect->wire->layer = 200;   /* dummy layer no */

sdfwritelay(SDFLAYALL, unconnect);
}


/* * * * * * * * *
 *
 * This routine adds an 'unconnect error' with name 'name'
 * atthe specified position
 */
add_error_unconnect(net, x, y, z)
NETPTR
   net;
GRIDADRESSUNIT
   x, y, z;  /* point */
{

if(unconnect != NULL)
   add_unconnect(((R_CELLPTR) net->circuit->flag.p)->layout, 
              net->name, x, y, z);


}


add_unconnect(lay, name, x, y, z)
LAYOUTPTR   
   lay;
char
   *name;
GRIDADRESSUNIT
   x, y, z;  /* point */
{ 
register LAYINSTPTR
   linst;
LAYOUTPTR
   errorcell; 
WIREPTR
   wire;
R_CELLPTR
   rcell;
int
   numinst;
char
   errname[128];

/* 
 * just add errors to layout
 */
if(unconnect == NULL)
   return;

if((rcell = (R_CELLPTR) lay->flag.p) == NULL)
   return;

/* 
 * just add errors to layout
 */
errorcell = lay;

if(x < rcell->cell_bbx.crd[L] ||
   x > rcell->cell_bbx.crd[R] ||
   y < rcell->cell_bbx.crd[B] ||
   y > rcell->cell_bbx.crd[T])
   {
/*   fprintf(stderr,"WARNING (add_unconnect): point ouside bbx\n"); */
   return;
   }

/*
 * instance already there??
 */
numinst = 1;
for(linst = errorcell->slice->chld.layinst;
    linst != NULL;
    linst = linst->next)
   {
   if(linst->layout == unconnect && linst->mtx[2] == x && linst->mtx[5] == y)
      break;
   if(strncmp(name, linst->name, (size_t) strlen(name)) == 0)
      numinst++;
   }

if(linst != NULL && strncmp(name, linst->name, (size_t) strlen(name)) == 0)
   return; /* already there */

/*
 * make instance of unconnect
 */
NewLayinst(linst);
/* name sure that name is unique: add number... */
/* disabled, too problemnatic.. */
/* sprintf(errname,"%s%d", name, numinst); */
sprintf(errname,"%s", name);
linst->name = canonicstring(errname);
linst->layout = unconnect;
linst->mtx[0] = 1; linst->mtx[1] = 0; linst->mtx[2] = x;
linst->mtx[3] = 0; linst->mtx[4] = 1; linst->mtx[5] = y;
linst->next = errorcell->slice->chld.layinst;
errorcell->slice->chld.layinst = linst;
}



/* * * * * * *
 *
 * This routine creates/finds an seadif cell.
 * NOLY cell fun cir lay (if it does not yet exist)
 */
LAYOUTPTR find_sdf_layout_cell(what, layout, circuit, function, library)
int
   what;      /* what is to be read */
char 
   *library, *function, *circuit, *layout;   /* names (canonicstringed) */
{ 
LIBRARYPTR
   lib;
register FUNCTIONPTR
   func;
register CIRCUITPTR
   cir;
register LAYOUTPTR
   lay;


/*
 * look in lib for cell
 */
if(existslib(library))
   {
   if(sdfreadlib(SDFLIBSTAT, library) == NULL)
      error(FATAL_ERROR, "WARNING (find_sdf_layout_cell): lib is there, but cannot read.");

   lib = thislib;
   } 
else
   { /* not found */
   NewLibrary(lib);
   lib->name = canonicstring(library);
   lib->technology = canonicstring(DEFAULT_TECHNOLOGY);
   /* no functions */
   NewStatus(lib->status);
   lib->status->timestamp = time(0);
   lib->status->program = canonicstring(DEFAULT_PROGRAM);
   lib->status->author = canonicstring(DEFAULT_AUTHOR);
   sdfwritelib(SDFLIBSTAT, lib);
   }

if(existsfun(function, library))
   {
   if(sdfreadfun(SDFFUNSTAT, function, library) == NULL)
      error(FATAL_ERROR, "WARNING (find_sdf_layout_cell): func is there, but cannot read.");

   func = thisfun;
   }
else
   { /* not found */
   NewFunction(func);
   func->name = canonicstring(function);
   func->type = canonicstring(DEFAULT_TYPE);
   /* no circuits yet */
   NewStatus(func->status);
   func->status->timestamp = time(0);
   func->status->program = canonicstring(DEFAULT_PROGRAM);
   func->status->author = canonicstring(DEFAULT_AUTHOR);
   func->library = lib;
   func->next = lib->function;
   lib->function = func;
   sdfwritefun(SDFFUNSTAT, func);
   }

/*
 * look for this circuit
 */
if(existscir(circuit, function, library))
   {
   if(sdfreadcir(SDFCIRSTAT, circuit, function, library) == NULL)
      error(FATAL_ERROR, "WARNING (find_sdf_layout_cell): cir is there, but cannot read.");

   cir = thiscir;
   }
else
   { /* empty */
   NewCircuit(cir);
   cir->name = canonicstring(circuit);
   /* no lists yet */
   NewStatus(cir->status);
   cir->status->timestamp = time(0);
   cir->status->program = canonicstring(DEFAULT_PROGRAM);
   cir->status->author = canonicstring(DEFAULT_AUTHOR);
   cir->function = func;
   cir->next = func->circuit;
   func->circuit = cir;
   sdfwritecir(SDFCIRSTAT, cir);
   }

if(existslay(layout, circuit, function, library))
   {
   if(sdfreadlay(what, layout, circuit, function, library) == NULL)
      error(FATAL_ERROR, "WARNING (find_sdf_layout_cell): lay is there, but cannot read.");

   lay = thislay;
   }
else
   {
   NewLayout(lay);
   lay->name = canonicstring(layout);
   NewStatus(lay->status);
   lay->status->timestamp = time(0);
   lay->status->program = canonicstring(DEFAULT_PROGRAM);
   lay->status->author = canonicstring(DEFAULT_AUTHOR);
   lay->circuit = cir;
   lay->next = cir->layout;
   cir->layout = lay;
   sdfwritelay(SDFLAYSTAT, lay);
   }

return(lay);
}

