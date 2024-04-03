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

static void *check_for_flag(char* programe_name, char *str, t_flag_context *flag_c, int *flags, int8_t *error)
{
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
                // get opt_node, check if he need value, if yes update it
            t_opt_node *opt = search_exist_opt(flag_c->opt_lst, is_same_char_opt, str[j]);
            if (opt && opt->has_value) {
                // ft_printf_fd(1, "need Update value of %c\n", str[j]);
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

/** 
 * @brief parse_flag
 * Parse user input to get flag
*/
int parse_flag(int argc, char **argv, t_flag_context *flag_c, int8_t *error)
{
    int flags = 0, char_skip = 0;
    t_opt_node *opt = NULL;
    char opt_val = '\0';
    int8_t in_search = 0;


    for (int i = 0; i < argc; ++i) {
        ft_printf_fd(1, YELLOW"Check str flag:argv[%d] %s\n"RESET,i, argv[i]);
        if (argv[i][0] == '-') {
            opt = check_for_flag(argv[0], argv[i], flag_c, &flags, error);
            if (*error == -1) {
                return (0);
            }
            if (opt) {
                ft_printf_fd(2, CYAN"upt opt->char = %c, last val %d -> ", opt->flag_char, opt->value);
                for (int j = 0; argv[i + j]; ++j) {
                    in_search = 1;
                    char_skip = (j == 0) ? 2 : 0;
                    opt_val = find_next_no_space(&argv[i + j][char_skip]);
                    if (opt_val != '\0') {
                        opt->value = ft_atoi(&argv[i + j][char_skip]);
                        i += j;
                        in_search = 0;
                        ft_printf_fd(2, "new val %u, j = %d, first off %d\n"RESET, opt->value, j, char_skip); 
                        break;
                    }
                }
            }
        }
    }
    ft_printf_fd(1, "hey search %d\n", in_search);
    if (in_search){
        ft_printf_fd(2, RESET""PARSE_FLAG_ERR_MSG_ARGS_REQ,  argv[0], opt->flag_char,  argv[0]);
        *error = -1;
    }

    display_flags(flag_c->opt_str, flags);
	return (flags);
}

