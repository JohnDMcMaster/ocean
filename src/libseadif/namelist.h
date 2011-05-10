/*
 * @(#)namelist.h 1.17 06/24/94 Delft University of Technology
 */
#ifndef __NAMELIST_H
#define __NAMELIST_H

typedef struct _NAMELIST
{
char             *name;
struct _NAMELIST *next;
}
NAMELIST,NAMELIST_TYPE,*NAMELISTPTR;


typedef struct _NAMELISTLIST
{
char                 *namelistname;
NAMELISTPTR          namelist;
struct _NAMELISTLIST *next;
}
NAMELISTLIST,NAMELISTLIST_TYPE,*NAMELISTLISTPTR;


#define NewNamelist(p) ((p)=(NAMELISTPTR)mnew(sizeof(NAMELIST_TYPE)))
#define NewNamelistlist(p) ((p)=(NAMELISTLISTPTR)mnew(sizeof(NAMELISTLIST_TYPE)))
#define FreeNamelist(p) \
   {forgetstring((p)->name); mfree((char **)(p),sizeof(NAMELIST_TYPE));}
#define FreeNamelistKeepString(p) \
   mfree((char **)(p),sizeof(NAMELIST_TYPE))
#define FreeNamelistlist(p) \
   {if ((p)->namelistname) fs((p)->namelistname); \
    mfree((char **)(p),sizeof(NAMELISTLIST_TYPE));}

typedef struct _NUM2LIST
{
long             num1,num2;
struct _NUM2LIST *next;
}
NUM2LIST,NUM2LIST_TYPE,*NUM2LISTPTR;


typedef struct _NUM2LISTLIST
{
char                 *num2listname;
NUM2LISTPTR          num2list;
struct _NUM2LISTLIST *next;
}
NUM2LISTLIST,NUM2LISTLIST_TYPE,*NUM2LISTLISTPTR;


#define NewNum2list(p) ((p)=(NUM2LISTPTR)mnew(sizeof(NUM2LIST_TYPE)))
#define NewNum2listlist(p) ((p)=(NUM2LISTLISTPTR)mnew(sizeof(NUM2LISTLIST_TYPE)))
#define FreeNum2list(p) \
   {mfree((char **)(p),sizeof(NUM2LIST_TYPE));}
#define FreeNum2listlist(p) \
   {if ((p)->num2listname) fs((p)->num2listname); \
    mfree((char **)(p),sizeof(NUM2LISTLIST_TYPE));}

#endif
