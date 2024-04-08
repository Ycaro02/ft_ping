#include "../include/ft_ping.h"

int g_signal_received = 0;


static void set_opt_value(t_list *opt_lst, uint32_t flag, uint32_t to_find, void *to_update)
{
	if (has_flag(flag, to_find)) {
		t_opt_node *opt = search_exist_opt(opt_lst, is_same_flag_val_opt, (void *)&to_find);
		// ft_printf_fd(1, "for tofind %u: opt: %p\n", to_find, opt);
		if (!opt) {
			return ;
		}
		if (opt->value_type == DECIMAL_VALUE) {
			*(uint32_t *)to_update = opt->val.digit;
		} else if (opt->value_type == HEXA_VALUE) {
			*(char **)to_update = ft_strdup(opt->val.str);
		}
	}
}

int8_t init_flag_context(int argc, char**argv, t_context *c)
{
	t_flag_context flag_c;
	int8_t ret = 0;

	uint8_t		*exit_code = &c->exit_code;

	ft_bzero(&flag_c, sizeof(t_flag_context));
	add_flag_option(&flag_c, HELP_FLAG_CHAR, HELP_OPTION, OPT_NO_VALUE, OPT_NO_VALUE, "help");
	add_flag_option(&flag_c, V_FLAG_CHAR, V_OPTION, OPT_NO_VALUE, OPT_NO_VALUE, "verbose");
	add_flag_option(&flag_c, C_FLAG_CHAR, C_OPTION, UINT32_MAX, DECIMAL_VALUE, "count");
	add_flag_option(&flag_c, T_FLAG_CHAR, T_OPTION, UINT8_MAX, DECIMAL_VALUE, "ttl");
	add_flag_option(&flag_c, P_FLAG_CHAR, P_OPTION, MAX_PATTERN_SIZE, HEXA_VALUE, "pattern");
	add_flag_option(&flag_c, TIMEOUT_FLAG_CHAR, TIMEOUT_OPTION, UINT8_MAX, DECIMAL_VALUE, "timeout");
	add_flag_option(&flag_c, LINGER_FLAG_CHAR, LINGER_OPTION, UINT8_MAX, DECIMAL_VALUE, "linger");
	add_flag_option(&flag_c, VERSION_FLAG_CHAR, VERSION_OPTION, OPT_NO_VALUE, OPT_NO_VALUE, "version");

	ret = call_flag_parser(&flag_c, argc, argv, &c->flag);
	if (ret) {
		display_option_list(flag_c); /* to remove*/
		set_opt_value(flag_c.opt_lst, c->flag, T_OPTION, &c->opt_value.ttl);
		set_opt_value(flag_c.opt_lst, c->flag, C_OPTION, &c->opt_value.count);
		set_opt_value(flag_c.opt_lst, c->flag, P_OPTION, &c->opt_value.pattern);
		set_opt_value(flag_c.opt_lst, c->flag, TIMEOUT_OPTION, &c->opt_value.timeout);
		set_opt_value(flag_c.opt_lst, c->flag, LINGER_OPTION, &c->opt_value.linger);
		free_flag_context(&flag_c);
		ft_printf_fd(1, ORANGE"ttl: %u\n"RESET, c->opt_value.ttl);
		ft_printf_fd(1, ORANGE"count: %u\n"RESET, c->opt_value.count);
		ft_printf_fd(1, ORANGE"pattern: %s len: %d\n"RESET, c->opt_value.pattern, ft_strlen(c->opt_value.pattern));
		ft_printf_fd(1, ORANGE"timeout: %u\n"RESET, c->opt_value.timeout);
		ft_printf_fd(1, ORANGE"linger: %u\n"RESET, c->opt_value.linger);
	}


	if (has_flag(c->flag, HELP_OPTION)) {
		ft_printf_fd(1, BONUS_HELP_MESSAGE, argv[0]);
		*exit_code = 0;
		return (FALSE);
	}


	return (ret);
}

/**
 *	@brief Main function
 *	@param argc number of arguments
 *	@param argv arguments, destination address and potential flag
*/
int main(int argc, char **argv)
{
    t_context       c;
    
    if (argc < 2) {
        ft_printf_fd(2, PURPLE"%s: usage error: Destination address required\n"RESET, argv[0]);
        return (1);
    }
    c = init_ping_context(argc, argv);
    if (c.send_sock == -1 || c.rcv_sock == -1) {
        goto free_context_label;
    } else if (!sending_ping_loop(&c)) {
        goto free_context_label;
    }
    /* If no error occur return code is 0*/
	c.exit_code = 0;

    /* Free socket label */
    free_context_label:
	if (c.summary.nb_send > 0) {
		display_clear_summary(&c);
	}
	free_context(&c);
    
    return (c.exit_code);
}