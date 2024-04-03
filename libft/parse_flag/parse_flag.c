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


int flag_value_long_format(t_flag_context *flag_c, char *full_name)
{
    t_opt_node *opt = NULL;
    int flag = -1;

    opt = search_exist_opt(flag_c->opt_lst, is_same_full_name, full_name);
    if (opt) {
        flag = (1 << opt->flag_val);
    }
    return (flag);

}


static void *check_for_flag(char* programe_name, char *str, t_flag_context *flag_c, int *flags, int8_t *error)
{
    t_opt_node *opt = NULL;
    int tmp_value = 0;
    int8_t already_present = 0;

    if (!str[1]) {
        ft_printf_fd(2, PARSE_FLAG_ERR_MSG,  programe_name, str[0],  programe_name);
        *error = -1;
    } else if (str[1] == '-' && str[2] == '\0') { /* special case to skip '--' fix with long option handling */
        return (NULL);
    } else {
        for (int j = 1; str[j]; ++j) {
            tmp_value =  get_flag_value(flag_c->opt_str, str[j]);
            if (tmp_value == -1) {
                ft_printf_fd(2, PARSE_FLAG_ERR_MSG,  programe_name, str[j],  programe_name);
                *error = -1;
                return (NULL);
            }
            already_present = flag_already_present(*flags, tmp_value);
            if (!already_present) {
                set_flag(flags, tmp_value);
            }
            opt = search_exist_opt(flag_c->opt_lst, is_same_char_opt, &str[j]);
            if (opt && opt->has_value) {
                return(opt);
            }
        }
    }
    return (NULL);
}

static int8_t is_char_space(char c) {
    return (c == ' ' || (c >= '\t' && c <= '\r'));
}

static char find_next_no_space(char *str) {
    int i = 0;

    while (str[i] && is_char_space(str[i])) {
        ++i;
    }
    return (str[i]);
}

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


uint32_t parse_flag_value(char *str) {
    uint32_t value = 0;
    if (str_is_digit(str)) {
        value = ft_atoi(str);
    }
    /* value * 1 if true othewise return 0 */
    value *= (value <= UINT8_MAX);
    return (value);
}


static int search_opt_value(char **argv, int *i, t_opt_node *opt)
{
    int char_skip = 0;
    int8_t in_search = 0;
    char opt_val = '\0';

    // ft_printf_fd(2, CYAN"update opt |%c|, curr val %d -> ", opt->flag_char, opt->value);
    for (int j = 0; argv[*i + j]; ++j) {
        in_search = 1;
        char_skip = ((j == 0) * 2);
        opt_val = find_next_no_space(&argv[*i + j][char_skip]);
        if (opt_val != '\0') {
            opt->value = parse_flag_value(&argv[*i + j][char_skip]);
            if (opt->value == 0) {
                ft_printf_fd(2, PARSE_FLAG_ERR_MSG_WRONG_ARGS,  argv[0], opt->flag_char, &argv[*i + j][char_skip],  argv[0]);
                return (FALSE);
            }
            argv[*i + j] = "";
            *i += j;
            in_search = 0;
            // ft_printf_fd(2, "new val %u, j = %d, first off %d\n"RESET, opt->value, j, char_skip); 
            break;
        }
        argv[*i + j] = "";
    }
    if (in_search){
        ft_printf_fd(2, RESET""PARSE_FLAG_ERR_MSG_ARGS_REQ,  argv[0], opt->flag_char,  argv[0]);
        return (FALSE);
    }
    return (TRUE);
}

/** 
 * @brief parse_flag
 * Parse user input to get flag
*/
int parse_flag(int argc, char **argv, t_flag_context *flag_c, int8_t *error)
{
    int flags = 0;
    t_opt_node *opt = NULL;

    for (int i = 1; i < argc; ++i) {
        ft_printf_fd(1, YELLOW"Check str flag:argv[%d] %s\n"RESET,i, argv[i]);
        if (argv[i][0] == '-') {
            /* if second char is '-' check for long format instead of char */

            opt = check_for_flag(argv[0], argv[i], flag_c, &flags, error);
            if (*error == -1) { /* if invalid flag return */
                return (FALSE);
            } else if (opt && !search_opt_value(argv, &i, opt)) {
                *error = -1;
                return (FALSE);
            }
        }
    }

    // for (int i = 0; i < argc; ++i) {
    //     ft_printf_fd(1, YELLOW"argv[%d] %s\n"RESET,i, argv[i]);
    // }

    display_flags(flag_c->opt_str, flags);
	return (flags);
}

