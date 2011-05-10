/*
 * @(#)functables.h 1.11 11/12/91 Delft University of Technology
 *
 * Defines table structures that enable fast searching for function
 * implementations with a specified property. The structures assume that
 * the types LAYOUTPTR and CIRCUITPTR are already known to the compiler.
 */

/*
 ***************************************************************************
 */

typedef struct
{
  short          aspect;	  /* Aspect ratio. */
  struct _LAYOUT *layout;	  /* Layout with specified aspect ratio. */
}
ASPECT,*ASPECTPTR;		  /* Basic element in this property table. */


typedef struct
{
  ASPECTPTR tabstart;
  long      tablength;
}
BIN_ASPECT,*BIN_ASPECT_PTR;	  /* Binary look-up for aspect ratio. */


/*
 ***************************************************************************
 */

typedef struct
{
  short          feeds_bt;	  /* Feed-throughs from bottom to top. */
  struct _LAYOUT *layout;	  /* Layout with specified feeds_bt. */
}
FEEDS_BT,*FEEDS_BTPTR;		  /* Basic element in this property table. */


typedef struct
{
  FEEDS_BTPTR tabstart;
  long        tablength;
}
BIN_FEEDS_BT,*BIN_FEEDS_BT_PTR;	  /* Binary look-up for feeds_bt. */

/*
 ***************************************************************************
 */

typedef struct
{
  short          feeds_lr;	  /* Feed-throughs from left to right. */
  struct _LAYOUT *layout;	  /* Layout with specified feeds_lr. */
}
FEEDS_LR,*FEEDS_LRPTR;		  /* Basic element in this property table. */


typedef struct
{
  FEEDS_LRPTR tabstart;
  long        tablength;
}
BIN_FEEDS_LR,*BIN_FEEDS_LR_PTR;	  /* Binary look-up for feeds_lr. */

/*
 ***************************************************************************
 */

typedef struct
{
  short          feeds_bl;	  /* Feed-throughs from bottom to left. */
  struct _LAYOUT *layout;	  /* Layout with specified feeds_bl. */
}
FEEDS_BL,*FEEDS_BLPTR;		  /* Basic element in this property table. */


typedef struct
{
  FEEDS_BLPTR tabstart;
  long        tablength;
}
BIN_FEEDS_BL,*BIN_FEEDS_BL_PTR;	  /* Binary look-up for feeds_bl struct. */

/*
 ***************************************************************************
 */

typedef struct
{
  short          feeds_br;	  /* Feed-throughs from bottom to right. */
  struct _LAYOUT *layout;	  /* Layout with specified feeds_br. */
}
FEEDS_BR,*FEEDS_BRPTR;		  /* Basic element in this property table. */


typedef struct
{
  FEEDS_BRPTR tabstart;
  long        tablength;
}
BIN_FEEDS_BR,*BIN_FEEDS_BR_PTR;	  /* Binary look-up for feeds_br. */

/*
 ***************************************************************************
 */

typedef struct
{
  short          feeds_tl;	  /* Feed-throughs from top to left. */
  struct _LAYOUT *layout;	  /* Layout with specified feeds_tl. */
}
FEEDS_TL,*FEEDS_TLPTR;		  /* Basic element in this property table. */


typedef struct
{
  FEEDS_TLPTR tabstart;
  long        tablength;
}
BIN_FEEDS_TL,*BIN_FEEDS_TL_PTR;	  /* Binary look-up for feeds_tl ratio. */

/*
 ***************************************************************************
 */

typedef struct
{
  short          feeds_tr;	  /* Feed-throughs from top to right. */
  struct _LAYOUT *layout;	  /* Layout with specified feeds_tr. */
}
FEEDS_TR,*FEEDS_TRPTR;		  /* Basic element in this property table. */


typedef struct
{
  FEEDS_TRPTR tabstart;
  long        tablength;
}
BIN_FEEDS_TR,*BIN_FEEDS_TR_PTR;	  /* Binary look-up for feeds_tr ratio. */

/*
 ***************************************************************************
 */

typedef struct
{
  short           style;	  /* Static cmos, nmos, domino, two phase,... */
  struct _CIRCUIT *circuit;	  /* Circuit of the specified design style. */
}
STYLE,*STYLEPTR;		  /* Basic element in the style hash table */


typedef struct
{
  STYLEPTR tabstart;
  long     tablength;
}
HASH_STYLE,*HASH_STYLE_PTR;	  /* Hash table for circuit styles. */

/*
 ***************************************************************************
 */


typedef union
{
  BIN_ASPECT_PTR    bin_aspect;	  /* Binary look-up for aspect ratio. */
  BIN_FEEDS_BT_PTR  bin_feeds_bt; /* Number of feed throughs, bottom to top. */
  BIN_FEEDS_LR_PTR  bin_feeds_lr; /* Number of feed throughs, left to right. */
  BIN_FEEDS_BL_PTR  bin_feeds_bl; /* Number of feed throughs, bottom to left. */
  BIN_FEEDS_BR_PTR  bin_feeds_br; /* Number of feed throughs, bottom to right.*/
  BIN_FEEDS_TL_PTR  bin_feeds_tl; /* Number of feed throughs, top to left. */
  BIN_FEEDS_TR_PTR  bin_feeds_tr; /* Number of feed throughs, top to right. */
  HASH_STYLE_PTR    hash_style;	  /* Static cmos, nmos, domino, two phase,... */
  /* et cetera... */
}
PTAB,PTAB_TYPE;			  /* Declares all recognized property tables. */


typedef struct _PROPTAB
{
  short           property;	  /* Table sorted with respect to this key. */
  short           tabletype;	  /* Type of table (binary,hash,linear,...) */
  PTAB            ptab;		  /* Reference to the actual table. */
  struct _PROPTAB *next;	  /* Next table with same or other property. */
}
PROPTAB,PROPTAB_TYPE,*PROPTABPTR; /* Enables fast searching for function
				   * implementations with known properties. */

