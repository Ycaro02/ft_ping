#include "../include/ft_ping.h"

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
	c.start = get_ms_time();
	c.exit_code = 1;
	c.send_sock = -1;
	c.rcv_sock = -1;
    c.src_addr = get_process_ipv4_addr();
    c.dest.sockaddr.sin_family = AF_INET;
	return (c);
}

int8_t call_flag_parser(t_flag_context *flag_c, int argc, char **argv, uint32_t *flag)
{
	int8_t flag_error = 0;

	/* get flag */
	*flag = parse_flag(argc, argv, flag_c, &flag_error);
	if (flag_error == -1) {
		free_flag_context(flag_c);
		return (FALSE);
	}
	return (TRUE);	
}

/**
 *	@brief Initialize ping context
 *	@param dest_addr destination address to send ping
*/
t_context init_ping_context(int argc, char **argv)
{
    t_context	c = init_context_structure(argc, argv);

	/* init flag context and parse cmd line to get flag in c.flag */	
	if (!init_flag_context(argc, argv, &c) ){
		return (c);
	}
	/* extract all args in c.str_args */
	c.str_args = extract_args(argc, argv); 


    c.send_sock = open_send_socket();
	if (c.opt_value.linger == 0){
		c.opt_value.linger = 1; /* Default time to wait response */
	}
    c.rcv_sock = open_rcv_socket(c.opt_value.linger);
    return (c);    
}

/**
 *	@brief free context structure field and close socket
*/
void free_context(t_context *c)
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
	if (c->opt_value.pattern) {
		free(c->opt_value.pattern);
	}


    close_multi_socket(c->rcv_sock, c->send_sock);
    DEBUG_PRINT(ORANGE"All ressources free\n"RESET);
}
