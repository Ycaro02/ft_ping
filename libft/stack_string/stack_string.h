#ifndef STACK_STRING_H
#define STACK_STRING_H

#include "../libft.h"


# define SSTRING_MAX 1024

typedef struct s_stack_string
{
    char        data[SSTRING_MAX];      /* char buffer */
    uint16_t    size;                   /* Current len of string, position of \0 */
}	t_stack_string;

typedef t_stack_string t_sstring;

t_sstring   empty_sstring();
t_sstring   fill_sstring(char *str);
t_sstring   fill_multiple_sstring(char *str1, char *str2, char *str3);
void        clear_sstring(t_sstring *sstr);
void        push_sstring(t_sstring *sstr, char c);
void        concat_sstring(t_sstring *sstr, char *str);

#endif /* STACK_STRING_H */