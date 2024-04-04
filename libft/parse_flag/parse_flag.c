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

/** 
 *	@brief Get flag value from char
 *	@param flag_c flag context
 *	@param c pointer on char to check
*/
static int get_flag_value(t_flag_context *flag_c, char *c) 
{
    int i = 0;
    int flag = -1;
	char *all_flag = flag_c->opt_str;

    while (all_flag && all_flag[i]) {
        if (*c == all_flag[i]) {
            flag = (1 << i);
            break;
        }
        ++i;
    }
    return (flag);
}

/**
 *	@brief Get flag value from full name
 *	@param flag_c flag context
 *	@param full_name pointer on full name to check
*/
int flag_value_long_format(t_flag_context *flag_c, char *full_name)
{
    t_opt_node *opt = NULL;
    int flag = -1;

    opt = search_exist_opt(flag_c->opt_lst, is_same_full_name, full_name);
	ft_printf_fd(2, "full_name: %s\n", full_name);
    if (opt) {
		ft_printf_fd(2, "FOUND opt->fullname: %s\n", opt->full_name);
	    flag = opt->flag_val;
    }
    return (flag);

}

/**
 *	@brief Check for add flag
 *	@param programe_name name of programe
 *	@param str pointer on string to check
 *	@param flag_c flag context
 *	@param flags pointer on flags to fill
 *	@param error pointer on error
 *	@param long_option long option or short option
 *	@return opt_node if found, NULL otherwise
*/
static void *check_for_flag(char* programe_name, char *str, t_flag_context *flag_c, int *flags, int8_t *error, uint8_t long_option)
{
    t_opt_node	*opt = NULL;
    int			tmp_value = 0;
	/* Function ptr to choice between char and long format function call */
	int			(*ptr_func)() = get_flag_value;
	int8_t		(*is_same_func)() = is_same_char_opt;
	int			j_start = 1;

	if (long_option) {
		ptr_func = flag_value_long_format;
		is_same_func = is_same_full_name;
		j_start = 2;
	}

    if (!str[j_start]) {
        ft_printf_fd(2, "j_start %d"PARSE_FLAG_ERR_MSG,  j_start, programe_name, &str[j_start],  programe_name);
        *error = -1;
		return (NULL);
    } 
	for (int j = j_start; str[j]; ++j) {
		ft_printf_fd(1, RED"str: %s\n"RESET, &str[j]);
		tmp_value = ptr_func(flag_c, &str[j]);
		if (tmp_value == -1) {
			ft_printf_fd(2, PARSE_FLAG_ERR_MSG,  programe_name, &str[j],  programe_name);
			*error = -1;
			return (NULL);
		}
		if (!flag_already_present(*flags, tmp_value)) {
			set_flag(flags, tmp_value);
		}
		opt = search_exist_opt(flag_c->opt_lst, is_same_func, &str[j]);
		if (opt) {
			break ;
		}
	}
    return (opt);
}

/** 
 * @brief Check if char is space
 * @param c char to check
 * @return 1 if space, 0 otherwise
*/
static int8_t is_char_space(char c) {
    return (c == ' ' || (c >= '\t' && c <= '\r'));
}

/** 
 * @brief Find next char that is not space
 * @param str string to check
 * @return next char that is not space
*/
static char find_next_no_space(char *str) {
    int i = 0;

    while (str[i] && is_char_space(str[i])) {
        ++i;
    }
    return (str[i]);
}

/**
 * @brief Parse flag value
 * @param str string to parse
 * @return value if valid, 0 otherwise
*/

uint32_t parse_flag_value(char *str) {
    uint32_t value = 0;
    if (str_is_digit(str)) {
        value = ft_atoi(str);
    }
    /* value * 1 if true othewise return 0 */
    value *= (value <= UINT8_MAX);
    return (value);
}

/**
 * @brief Search for value linked to flag
 * @param argv pointer on argv
 * @param i pointer on index
 * @param opt pointer on opt node to update
 * @return 1 if found, 0 otherwise
*/
static int search_opt_value(char **argv, int *i, t_opt_node *opt, uint8_t long_format_bool)
{
    int char_skip = 0;
    int8_t in_search = 0;
    char opt_val = '\0';
	int to_skip_idx = 2;

	if (long_format_bool == LONG_FORMAT) {
		to_skip_idx += ft_strlen(opt->full_name);
	}

    // ft_printf_fd(2, CYAN"update opt |%c|, curr val %d -> ", opt->flag_char, opt->value);
    for (int j = 0; argv[*i + j]; ++j) {
        in_search = 1;
        char_skip = ((j == 0) * to_skip_idx);
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
 * @brief Parse flag
 * @param argc number of argument
 * @param argv pointer on argument
 * @param flag_c pointer on flag context
 * @param error pointer on error
 * @return flags if valid, 0 otherwise and set error to -1
*/
int parse_flag(int argc, char **argv, t_flag_context *flag_c, int8_t *error)
{
    int flags = 0;
    t_opt_node *opt = NULL;
	uint8_t long_format_bool = CHAR_FORMAT;
	// int func_find_flag = get_flag_value;

    for (int i = 1; i < argc; ++i) {
        ft_printf_fd(1, YELLOW"Check str flag:argv[%d] %s\n"RESET,i, argv[i]);
        if (argv[i][0] == '-') {
            /* if second char is '-' check for long format instead of char */
			if (argv[i][1] && argv[i][1] == '-') {
				ft_printf_fd(1, "Long format for |%s|\n", argv[i]);
				long_format_bool = LONG_FORMAT;
            	// opt = check_for_flag(argv[0], &argv[i][1], flag_c, &flags, error, LONG_FORMAT);
			} 

			opt = check_for_flag(argv[0], argv[i], flag_c, &flags, error, long_format_bool);
            if (*error == -1) { /* if invalid flag return */
                return (FALSE);
            } else if (opt && opt->has_value && !search_opt_value(argv, &i, opt, long_format_bool)) {
                *error = -1;
                return (FALSE);
            }
        }
		/* Reset long format bool */
		long_format_bool = CHAR_FORMAT;
    }

    // for (int i = 0; i < argc; ++i) {
    //     ft_printf_fd(1, YELLOW"argv[%d] %s\n"RESET,i, argv[i]);
    // }

    display_flags(flag_c->opt_str, flags);
	return (flags);
}

