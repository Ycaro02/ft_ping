# include "parse_flag.h"

# define COLOR_MAX 7

void display_flags(char *all_flag, int flags) {
    const char  *flag_colors[COLOR_MAX] = {RED,GREEN,YELLOW,PINK,CYAN,ORANGE,PURPLE};
    const char  *str_suffix = "_OPTION";

    ft_printf_fd(2, FILL_YELBLACK"Flags: [%d] : "RESET, flags);
    for (int i = 0; all_flag && all_flag[i]; ++i) {
        if ((flags >> i) & 1) {
            ft_printf_fd(2, "%s[%c%s]%s, ", flag_colors[i % COLOR_MAX], all_flag[i], str_suffix, RESET);
        }
    }
    ft_printf_fd(2, "\n");
}

/** get_flag_value
 * Convert char to flag value see ALL_FLAG and e_flag, define_enum.c
*/
static int get_flag_value(char *all_flag, char c) 
{
    int i = 0;
    int flag = -1;

    while (all_flag && all_flag[i]) {
        if (c == all_flag[i]) {
            flag = (1 << i);
            break;
        }
        ++i;
    }
    return (flag);
}

static void check_for_flag(char* programe_name, char *str, t_flag_context *flag_c, int *flags, int8_t *error)
{
    int tmp_value = 0;

    if (!str[1]) {
        ft_printf_fd(2, PARSE_FLAG_ERR_MSG,  programe_name, str[0],  programe_name);
        *error = -1;
    } else if (str[1] == '-' && str[2] == '\0') { /* special case to skip '--' fix with long option handling */
        return ;
    } else {
        for (int j = 1; str[j]; ++j) {
            tmp_value =  get_flag_value(flag_c->opt_str, str[j]);
            if (tmp_value == -1) {
                ft_printf_fd(2, PARSE_FLAG_ERR_MSG,  programe_name, str[j],  programe_name);
                *error = -1;
            } else if (flag_already_present(*flags, tmp_value) == FALSE) {
                set_flag(flags, tmp_value);
            }
        }
    }
}

/** 
 * @brief parse_flag
 * Parse user input to get flag
*/
int parse_flag(int argc, char **argv, t_flag_context *flag_c, int8_t *error)
{
    int flags = 0;

    for (int i = 0; i < argc; ++i) {
        if (argv[i][0] == '-') {
            check_for_flag(argv[0], argv[i], flag_c, &flags, error);
        }
    }
    display_flags(flag_c->opt_str, flags);
	return (flags);
}

