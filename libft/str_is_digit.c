#include "libft.h"

int8_t str_is_digit(char *str) 
{
    if (!str || !str[0]) {
        return (FALSE);
    }
    for (int i = 0; str[i]; ++i) {
        if (!ft_isdigit(str[i])) {
            return (FALSE);
        }
    }
    return (TRUE);
}
