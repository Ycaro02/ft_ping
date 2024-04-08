#ifndef STACK_STRING_H
#define STACK_STRING_H

#include "../libft.h"

/**
 * Basic stack string implementation, using a fixed size buffer
 * This is useful for small strings, and avoid dynamic allocation
 * Written by Ycaro
*/

/* Buffer size of stack string */
#define SSTRING_MAX 1024

/**
 * Stack string structure
*/
typedef struct s_stack_string
{
    char        data[SSTRING_MAX];      /* char buffer */
    uint16_t    size;                   /* Current len of string, position of \0 */
}	t_stack_string;

typedef t_stack_string t_sstring;

/**
 * @brief Create an empty stack string
*/
t_sstring   empty_sstring();

/**
 * @brief Fill a stack string with a string
 * @param str The string to fill the stack string with
*/
t_sstring   fill_sstring(char *str);

/**
 * @brief Fill a stack string with multiple strings
 * @param str1 The first string to fill the stack string with
 * @param str2 The second string to fill the stack string with
 * @param str3 The third string to fill the stack string with
*/

t_sstring   fill_multiple_sstring(char *str1, char *str2, char *str3);

/**
 * @brief Clear a stack string
 * @param sstr Pointer on the stack string to clear
*/
void        clear_sstring(t_sstring *sstr);

/**
 * @brief Push a char in a stack string
 * @param sstr Pointer on the stack string to push the char in
*/
void        push_sstring(t_sstring *sstr, char c);

/**
 * @brief Concatenate a string to a stack string
 * @param sstr Pointer on the stack string to concatenate the string to
 * @param str The string to concatenate
*/
void        concat_sstring(t_sstring *sstr, char *str);

#endif /* STACK_STRING_H */