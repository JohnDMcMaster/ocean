/*
 * @(#)namelist.c 1.18 06/24/94 Delft University of Technology
 */

#include "sealib.h"
#include "sea_decl.h"
#include "namelist.h"
#include <string.h>

void tonamelist(NAMELISTPTR *namelist, char *name)
{
   NAMELISTPTR l;

   NewNamelist(l);
   l->name=name;
   l->next= *namelist;
   *namelist=l;
}


void fromnamelist(NAMELISTPTR *namelist, char *name)
{
   NAMELISTPTR l1,l2;

   for (l1=l2= *namelist; l1!=NIL; l1=(l2=l1)->next)
      if (l1->name==name)
         break;
   if (l1!=NIL)
   {
      if (l1==l2)
         *namelist=l1->next;
      else
         l2->next=l1->next;
      FreeNamelist(l1);
   }
}


int isinnamelist(NAMELISTPTR namelist, char *name)
{
   NAMELISTPTR l;

   for (l=namelist; l!=NIL; l=l->next)
      if (l->name==name)
         break;
   return l!=NIL;
}


int isemptynamelist(NAMELISTPTR namelist)
{
   return namelist==NIL;
}


void tonamelistlist(NAMELISTLISTPTR *namelistlist, NAMELISTPTR namelist,
		    char *namelistname)
{
   NAMELISTLISTPTR p;

   /* Check to see that namelist is not already in the namelistlist. */
   for (p= *namelistlist; p!=NIL; p=p->next)
      if (p->namelistname==namelistname)
         return;
   NewNamelistlist(p);
   p->namelistname=namelistname;
   p->next= *namelistlist;
   p->namelist=namelist;
   *namelistlist=p;
}


/* convert a path of the form "str1:str2:str3" to a namelist containing
 * the strings (str1, str2, str3 str4)
 */   
void sdfpathtonamelist(NAMELISTPTR *nl, char *path)
{
   char *name,*endofname;
   *nl=NIL;			  /* set the namelist to NIL */
   if (path==NIL)
   {
      *nl=NIL;
      return;
   }
   do
   {
      while (*path == ':')		  /* skip surplus of ':' characters */
         ++path;
      name=path;			  /* set name to the start of the name */
      endofname=strchr(path,':');	  /* set endofname to the next occurenc of ':' */
      if (endofname != NIL)
      {
         *endofname='\0';		  /* make ':' end of string */
         path=endofname+1;		  /* set path to start of next name */
      }
      if (*name != '\0')
         tonamelist(nl,abscanonicpath(name)); /* add the name to the namelist */
   }
   while (endofname != NIL);
}



void tonum2list(NUM2LISTPTR *num2list, long num1, long num2)
{
   NUM2LISTPTR l;

   NewNum2list(l);
   l->num1=num1;
   l->num2=num2;
   l->next= *num2list;
   *num2list=l;
}


void fromnum2list(NUM2LISTPTR *num2list, long num1, long num2)
{
   NUM2LISTPTR l1,l2;

   for (l1=l2= *num2list; l1!=NIL; l1=(l2=l1)->next)
      if (l1->num1==num1 && l1->num2==num2)
         break;
   if (l1!=NIL)
   {
      if (l1==l2)
         *num2list=l1->next;
      else
         l2->next=l1->next;
      FreeNum2list(l1);
   }
}


int isinnum2list(NUM2LISTPTR num2list, long num1, long num2)
{
   NUM2LISTPTR l;

   for (l=num2list; l!=NIL; l=l->next)
      if (l->num1==num1 && l->num2==num2)
         break;
   return l!=NIL;
}


int isemptynum2list(NUM2LISTPTR num2list)
{
   return num2list==NIL;
}


void tonum2listlist(NUM2LISTLISTPTR *num2listlist,NUM2LISTPTR num2list,
		    char  *num2listname)
{
   NUM2LISTLISTPTR p;

   /* Check to see that num2list is not already in the num2listlist. */
   for (p= *num2listlist; p!=NIL; p=p->next)
      if (p->num2listname==num2listname)
         return;
   NewNum2listlist(p);
   p->num2listname=num2listname;
   p->next= *num2listlist;
   p->num2list=num2list;
   *num2listlist=p;
}
