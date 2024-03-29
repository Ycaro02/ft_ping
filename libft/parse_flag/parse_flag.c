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

/** 
 * @brief parse_flag
 * Parse user input to get flag
*/
int parse_flag(int argc, char **argv, int8_t *error)
{
    int i = 1, flags = 0, tmp_value = 0;

    while (i < argc) {
        if (argv[i][0] == '-') {
            if (!argv[i][1]) {   /* special case ugly */
                ft_printf_fd(2, "%s: cannot access %s: No such file or directoryn", argv[0], argv[i]);
            }
            else if (argv[i][1] == '-' && argv[i][2] == '\0') { /* again special case to skip '--' */
                ++i;
                continue;
            }
            else {
                for (int j = 1; argv[i][j]; ++j) {
                    tmp_value =  get_flag_value(argv[i][j]);
                    if (tmp_value == -1) {
                        ft_printf_fd(2, "%s: invalid option -- %c\nTry ./%s --help for more information\n",  argv[0], argv[i][j],  argv[0]);
                        *error = -1;
                        return (0);
                    }
                    if (flag_already_present(flags, tmp_value) == FALSE)
                        set_flag(&flags, tmp_value);
                }
            }
        }
        ++i;
    }
    display_flags(flags);
	return (flags);
}

