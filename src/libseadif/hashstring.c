/*
 *	@(#)hashstring.c 1.2 11/02/92 Delft University of Technology
 */

#include <sea_decl.h>

/* The function sdfhashstringtolong() converts a string to
 * a pseudo random integer in the range [0..tabsize-1].
 */
PUBLIC long sdfhashstringtolong(char *str,long tabsize)
{
long  sum,c,i,spec1=0;
union {long l; char c[4];} spec;

spec.l=0;
for (sum=0; (c=(*str))!=NULL; ++str)
   {
   sum+=c;
   spec1^=c;
   }
spec.c[2]=(char)spec1;
i=(long)((spec.l+sum+(sum<<3)+(sum<<5)+(sum<<19))%tabsize);
return(i<0 ? 0-i : i);
}



/* The function sdfhash2stringstolong() converts two strings
 * to a pseudo random integer in the range [0..tabsize-1].
 */
PUBLIC long sdfhash2stringstolong(char *str1,char *str2,long tabsize)
{
long  sum,c,i,spec1=0,spec2=0;
union {long l; char c[4];} spec;

spec.l=0;
for (sum=0; (c=(*str1))!=NULL; ++str1)
   {
   sum+=c;
   spec1^=c;
   }
for (; (c=(*str2))!=NULL; ++str2)
   {
   sum+=c;
   spec2^=c;
   }
spec.c[3]=(char)spec1; spec.c[2]=(char)spec2;
i=(long)((spec.l+sum+(sum<<3)+(sum<<5)+(sum<<19))%tabsize);
return(i<0 ? 0-i : i);
}




/* The function sdfhash3stringstolong() converts three strings
 * to a pseudo random integer in the range [0..tabsize-1].
 */
PUBLIC long sdfhash3stringstolong(char *str1,char *str2,char *str3,long tabsize)
{
long   sum,c,i,spec1=0,spec2=0,spec3=0;
union {long l; char c[4];} spec;

spec.l=0;
for (sum=0; (c=(*str1))!=NULL; ++str1)
   {
   sum+=c;
   spec1^=c;
   }
for (; (c=(*str2))!=NULL; ++str2)
   {
   sum+=c;
   spec2^=c;
   }
for (; (c=(*str3))!=NULL; ++str3)
   {
   sum+=c;
   spec3^=c;
   }
spec.c[3]=(char)spec1; spec.c[2]=(char)spec2; spec.c[1]=(char)spec3;
i=(long)((spec.l+sum+(sum<<3)+(sum<<5)+(sum<<19))%tabsize);
return(i<0 ? 0-i : i);
}



/* The function sdfhash4stringstolong() converts four strings
 * to a pseudo random integer in the range [0..tabsize-1].
 */
PUBLIC long sdfhash4stringstolong(char *str1,char *str2,char  *str3,char  *str4,
                   long tabsize)
{
long sum,c,i,spec1=0,spec2=0,spec3=0,spec4=0;
union {long l; char c[4];} spec;

spec.l=0;
for (sum=0; (c=(*str1))!=NULL; ++str1)
   {
   sum+=c;
   spec1^=c;
   }
for (; (c=(*str2))!=NULL; ++str2)
   {
   sum+=c;
   spec2^=c;
   }
for (; (c=(*str3))!=NULL; ++str3)
   {
   sum+=c;
   spec3^=c;
   }
for (; (c=(*str4))!=NULL; ++str4)
   {
   sum+=c;
   spec4^=c;
   }
spec.c[3]=(char)spec1; spec.c[2]=(char)spec2;
spec.c[1]=(char)spec3; spec.c[0]=(char)spec4;
i=(long)((spec.l+sum+(sum<<3)+(sum<<5)+(sum<<19))%tabsize);
return(i<0 ? 0-i : i);
}
