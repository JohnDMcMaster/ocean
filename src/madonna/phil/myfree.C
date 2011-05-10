// 	@(#)myfree.C 1.2 11/06/92 
//
#include <malloc.h>

void myfree(void *ptr)
{
#ifdef sparc
   free((char *)ptr);
#else
   free(ptr);
#endif
}
