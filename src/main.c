#include "../include/ft_ping.h"

int g_signal_received = 0;

int8_t init_flag_context(int argc, char**argv, uint16_t *flag, uint8_t *exit_code)
{
	t_flag_context flag_c;
	int8_t flag_error = 0;

	ft_bzero(&flag_c, sizeof(t_flag_context));
	add_flag_option(&flag_c, H_FLAG_CHAR, H_OPTION, OPT_NO_VALUE, "help");
	add_flag_option(&flag_c, V_FLAG_CHAR, V_OPTION, OPT_NO_VALUE, "verbose");
	add_flag_option(&flag_c, C_FLAG_CHAR, C_OPTION, OPT_HAS_VALUE, "count");
	add_flag_option(&flag_c, T_FLAG_CHAR, T_OPTION, OPT_HAS_VALUE, "ttl");

	/* get flag */
	*flag = parse_flag(argc, argv, &flag_c, &flag_error);
	if (flag_error == -1) {
		free_flag_context(&flag_c);
		return (FALSE);
	}

	display_option_list(flag_c);

	free_flag_context(&flag_c);

	if (has_flag(*flag, H_OPTION)) {
		ft_printf_fd(1, MANDATORY_HELP_MESSAGE, argv[0]);
		*exit_code = 0;
		return (FALSE);
	}

	return (TRUE);
}


/**
 * @brief Initialize ping context
 * @param argc number of arguments
 * @param argv arguments, destination address and potential flag
 * @return t_context initialized context
*/
t_context init_context_structure()
{
	t_context	c;
    ft_bzero(&c, sizeof(t_context));
	c.exit_code = 1;
	c.send_sock = -1;
	c.rcv_sock = -1;
    c.src_addr = get_process_ipv4_addr();
    c.dest.sockaddr.sin_family = AF_INET;
	return (c);
}

/**
 *	@brief Initialize ping context
 *	@param dest_addr destination address to send ping
*/
t_context init_ping_context(int argc, char **argv)
{
    t_context	c = init_context_structure(argc, argv);

	/* init flag context and parse cmd line to get flag in c.flag */	
	if (!init_flag_context(argc, argv, &c.flag, &c.exit_code) ){
		return (c);
	}
	/* extract all args in c.str_args */
	c.str_args = extract_args(argc, argv); 

	// for (t_list *current = c.str_args; current; current = current->next) {
	// 	ft_printf_fd(1, "arg: %s\n", (char *)current->content);
	// }
    c.send_sock = open_send_socket();
    c.rcv_sock = open_rcv_socket();
    return (c);    
}


static void free_context(t_context *c)
{
	if (c->summary.rcv_time_lst) {
		ft_lstclear(&c->summary.rcv_time_lst, free);
	}
	if (c->dest.name) {
		free(c->dest.name);
	}
	if (c->str_args) {
		ft_lstclear(&c->str_args, free);
	}

    close_multi_socket(c->rcv_sock, c->send_sock);
    ft_printf_fd(1, ORANGE"All ressources free\n"RESET);
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