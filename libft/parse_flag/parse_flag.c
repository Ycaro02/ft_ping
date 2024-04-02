# include "parse_flag.h"

void display_flags(int flags) {
    const char *flag_strings[NB_FLAG] = {"V_OPTION","C_OPTION","T_OPTION"};
    const char *flag_colors[NB_FLAG] = {GREEN,YELLOW,RED};

    ft_printf_fd(2, FILL_YELBLACK"Flags: [%d] : "RESET, flags);
    for (int i = 0; i < NB_FLAG; ++i) {
        if ((flags >> i) & 1) {
            ft_printf_fd(2, "%s[%s]%s, ", flag_colors[i], flag_strings[i], RESET);
        }
    }
    ft_printf_fd(2, "\n");
}

/** get_flag_value
 * Convert char to flag value see ALL_FLAG and e_flag, define_enum.c
*/
static int get_flag_value(char c) 
{
    int i = 0;
    int flag = -1;

    while (i < NB_FLAG) {
        if (c == ALL_FLAG[i]) {
            flag = (1 << i);
            break;
        }
        ++i;
    }
    return (flag);
}

static void check_for_flag(char* programe_name, char *str, int *flags, int8_t *error)
{
    int tmp_value = 0;

    if (!str[1]) {
        ft_printf_fd(2, PARSE_FLAG_ERR_MSG,  programe_name, str[0],  programe_name);
        *error = -1;
    } else if (str[1] == '-' && str[2] == '\0') { /* again special case to skip '--' fix with long option handling */
        return ;
    } else {
        for (int j = 1; str[j]; ++j) {
            tmp_value =  get_flag_value(str[j]);
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
int parse_flag(int argc, char **argv, int8_t *error)
{
    int flags = 0;

    for (int i = 0; i < argc; ++i) {
        if (argv[i][0] == '-') {
            check_for_flag(argv[0], argv[i], &flags, error);
        }
    }

    display_flags(flags);
	return (flags);
}

