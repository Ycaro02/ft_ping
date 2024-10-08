#include "../include/ft_ping.h"
/**
 * @brief signal handler function for SIGNINT
*/
int g_signal_received = 0;

/**
 * @brief Init mandatory flag context
 * @param argc number of arguments
 * @param argv arguments
 * @param c context to initialize
 */
int8_t init_flag_context(int argc, char**argv, t_context *c)
{
	t_flag_context flag_c;
	int8_t ret = 0;

	uint8_t		*exit_code = &c->exit_code;

	ft_bzero(&flag_c, sizeof(t_flag_context));
	add_flag_option(&flag_c, HELP_FLAG_CHAR, HELP_OPTION, OPT_NO_VALUE, OPT_NO_VALUE, "help");
	add_flag_option(&flag_c, V_FLAG_CHAR, V_OPTION, OPT_NO_VALUE, OPT_NO_VALUE, "verbose");
	
	ret = call_flag_parser(&flag_c, argc, argv, &c->flag);
	if (ret) {
		#ifdef DEBUG
			display_option_list(flag_c); /* to remove*/
		#endif
		free_flag_context(&flag_c);
	}

	if (has_flag(c->flag, HELP_OPTION)) {
		ft_printf_fd(1, MANDATORY_HELP_MESSAGE, argv[0]);
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