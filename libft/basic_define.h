#ifndef BASIC_DEF_TYPE_HEADER
# define BASIC_DEF_TYPE_HEADER

# include <sys/types.h>

/********************************************************************
 *                        DEFINE		                            *
 ********************************************************************/
/* bool */
# define    FALSE   0
# define    TRUE 	1
/* pointer size */
# define    PTR_SIZE	    sizeof(void *)
/* LS linux color */

# define    RED     "\x1b[31m"
# define    GREEN   "\x1b[32m"
# define    YELLOW  "\x1b[33m"
# define    BLUE    "\x1b[34m"
# define    PURPLE  "\x1b[35m"
# define    CYAN    "\x1b[36m"
# define    PINK    "\x1b[38;5;200m"
# define    ORANGE  "\x1b[38;5;208m"
# define    RESET   "\x1b[0m"


# define    FILL_GREEN      "\033[7;32m"
# define    FILL_YELLOW     "\033[7;33m"
# define    FILL_RED        "\033[7;31m"
# define    FILL_YELBLACK   "\033[40;33m"


 # endif /* BASIC_DEF_TYPE_HEADER */