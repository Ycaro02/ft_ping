#ifndef PARSE_FLAG_H
#define PARSE_FLAG_H 1

#include "../libft.h"

#define H_FLAG_CHAR 'h'         /* 0*/
#define	V_FLAG_CHAR 'v'         /* 1 */ 
#define	C_FLAG_CHAR 'c'			/* 2 */
#define	T_FLAG_CHAR 't'			/* 3 */

enum ping_flag  {
    UNKNOW=0, 
    H_OPTION=1,
    V_OPTION=2,
	C_OPTION=4,
	T_OPTION=8,
};

# define PARSE_FLAG_ERR_MSG RED"%s: invalid option -- %c\nTry ./%s -h for more information\n"RESET

# define PARSE_FLAG_ERR_MSG_ARGS_REQ RED"%s: option requires an argument -- %c\nTry ./%s -h for more information\n"RESET

#define OPT_NO_VALUE    0U
#define OPT_HAS_VALUE   1U

typedef struct opt_node {
    uint8_t             flag_char;  /* char represent flag */
    uint32_t            flag_val;   /* flag value, used with bitwise to create application flag */
    uint32_t            value;      /* value if value is linked */
    uint8_t             has_value;  /* if value is linked */
    // char                *full_name; /* full name opt */
}   t_opt_node;

typedef struct flag_context {
    char        *opt_str;       /* full char opt */
    t_list      *opt_lst;       /* list of opt node */
}   t_flag_context;

/* parse cmd_line */
t_list  *extract_args(int argc, char **argv);

/* parse flag*/
// int     parse_flag(int argc, char **argv, int8_t *reject_all);
int     parse_flag(int argc, char **argv,t_flag_context *flag_c, int8_t *error);
void display_flags(char *all_flag, int flags);
/* handle flag */
void    set_flag(int *flags, int flag_val);
void    unset_flag(int *flags, int flag_val);
int8_t  has_flag(int flags, int flag_val);
int8_t  flag_already_present(int flags, int flag_val);

/* handle option */
int8_t add_flag_option(t_flag_context *flag_c, uint8_t c, uint32_t flag_val, uint32_t value);
int8_t is_same_char_opt(void *content, uint32_t c);
int8_t is_same_flag_val_opt(void *content, uint32_t value);
void *search_exist_opt(t_list *opt_lst, int8_t (cmp(void *, uint32_t)), uint32_t test);
void free_flag_context(t_flag_context *flag_c);
void display_option_list(t_flag_context *flag_c, t_list *opt_lst);
void free_flag_context(t_flag_context *flag_c);

#endif /* PARSE_FLAG_H */