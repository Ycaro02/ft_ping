#include "../include/ft_ping.h"

int g_signal_received = 0;

int8_t init_flag_context(int argc, char**argv, uint16_t *flag, uint8_t *exit_code)
{
	t_flag_context flag_c;
	int8_t ret = 0;

	ft_bzero(&flag_c, sizeof(t_flag_context));
	add_flag_option(&flag_c, H_FLAG_CHAR, H_OPTION, OPT_NO_VALUE, "help");
	add_flag_option(&flag_c, V_FLAG_CHAR, V_OPTION, OPT_NO_VALUE, "verbose");
	
	ret = call_flag_parser(&flag_c, argc, argv, flag, exit_code);
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