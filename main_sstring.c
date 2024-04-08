#include "libft/stack_string/stack_string.h"

int main(int argc, char **argv)
{
    t_sstring sstr = empty_sstring();

    ft_printf_fd(1, "empty_sstring: %s\n", sstr.data);

    sstr = fill_sstring("Hello, ");
    ft_printf_fd(1, "after fill: %s\n", sstr.data);
    return (0);

}