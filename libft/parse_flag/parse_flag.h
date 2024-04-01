#ifndef PARSE_FLAG_H
#define PARSE_FLAG_H 1

#include "../libft.h"

#define	V_FLAG_CHAR 'v'         /* 0 */ 
#define	C_FLAG_CHAR 'c'			/* 1 */
#define	T_FLAG_CHAR 't'			/* 2 */
#define NB_FLAG		3			/* max flag str index*/
#define	ALL_FLAG	"vct"		/* all flag str */

enum ping_flag  {
    UNKNOW=0, 
    V_OPTION=1,
	C_OPTION=2,
	T_OPTION=4,
};

/* parse cmd_line */
t_list  *extract_args(int argc, char **argv);

/* parse flag*/
int     parse_flag(int argc, char **argv, int8_t *reject_all);
void    display_flags(int flags);

/* handle flag */
void    set_flag(int *flags, int flag_val);
void    unset_flag(int *flags, int flag_val);
int8_t  has_flag(int flags, int flag_val);
int8_t  flag_already_present(int flags, int flag_val);

/* Need to refactor flag festion to accept flag + value
    For this create a API for create/add node to linked list
    of accepted flag, need to take the letters to add and value (-1 for no value/classic flag) 

*/


#endif /* PARSE_FLAG_H */