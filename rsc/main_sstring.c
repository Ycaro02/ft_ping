#include "libft/stack_string/stack_string.h"

int main(int argc, char **argv)
{
    t_sstring sstr = empty_sstring();

    ft_printf_fd(1, "empty_sstring: %s\n", sstr.data);

    sstr = fill_sstring("Hello, ");
    ft_printf_fd(1, "after fill: %s\n", sstr.data);

    concat_sstring(&sstr, "world!");
    ft_printf_fd(1, "after concat: %s\n", sstr.data);
    
    push_sstring(&sstr, '|');
    ft_printf_fd(1, "after push: %s\n", sstr.data);

    clear_sstring(&sstr);
    ft_printf_fd(1, "after clear: %s\n", sstr.data);

    sstr = fill_multiple_sstring("HI, ", "KOALA", "!");
    ft_printf_fd(1, "after fill_multiple: %s\n", sstr.data);

    return (0);

}