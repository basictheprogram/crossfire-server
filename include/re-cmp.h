/* re-cmp.h
 * Datastructures for representing a subset of regular expressions.
 *
 * Author: Kjetil T. Homme <kjetilho@ifi.uio.no> May 1993
 */

/*   C o n f i g u r a t i o n
 */

#define SAFE_CHECKS	/* Regexp's with syntax errors will core dump if
			 * this is undefined.
			 */

#define RE_TOKEN_MAX 64	/* Max amount of tokens in a regexp.
			 * Each token uses ~264 bytes. They are allocated
			 * as needed, but never de-allocated.
			 * E.g. [A-Za-z0-9_] counts as one token, so 64
			 * should be plenty for most purposes.
			 */

/*   D o   n o t   c h a n g e    b e l o w
 */

#ifdef uchar
#    undef uchar
#endif
#ifdef Boolean
#    undef Boolean
#endif
#ifdef True
#    undef True
#endif
#ifdef False
#    undef False
#endif

#define uchar	unsigned char
#define Boolean	uchar
#define True	1	/* Changing this value will break the code */
#define False	0

typedef enum {
    sel_any, 		/* corresponds to e.g. .		*/
    sel_end,		/*         "           $		*/
    sel_single, 	/*         "           q		*/
    sel_range,  	/*         "           [A-F]		*/
    sel_array,  	/*         "           [AF-RqO-T]	*/
    sel_not_single,	/*         "           [^f]		*/
    sel_not_range	/*	   "           [^A-F]		*/
} selection_type;

typedef enum {
    rep_once,		/* corresponds to no meta-char	*/
    rep_once_or_more,	/*        "       +		*/
    rep_null_or_once,	/*        "       ?		*/
    rep_null_or_more	/*        "       *		*/
} repetetion_type;

typedef struct {
    selection_type type;
    union {
	uchar single;
	struct {
	    uchar low, high;
	} range;
	Boolean array[UCHAR_MAX];
    } u;
    repetetion_type repeat;
} selection;
