#include "../include/ft_ping.h"

int g_signal_received = 0;

/**
 *	@brief Initialize ping context
 *	@param dest_addr destination address to send ping
*/
t_context init_ping_context(char *dest_addr)
{
    t_context c;

    ft_bzero(&c, sizeof(t_context));
    c.src_addr = get_process_ipv4_addr();
    c.dst_sockaddr.sin_family = AF_INET;
    c.dst_sockaddr.sin_addr.s_addr = ipv4_str_toaddr(dest_addr);
    c.send_sock = open_send_socket();
    c.rcv_sock = open_rcv_socket();

    return (c);    
}


void display_ms_time(char *color, suseconds_t time, uint8_t last)
{
	uint32_t mod = time % 1000;

	if (mod < 100) {
		ft_printf_fd(1, "%s%i.0%i", color, (time / 1000), mod);
	} else if (mod < 10) {
		ft_printf_fd(1, "%s%i.00%i", color, (time / 1000), mod);
	} else {
		ft_printf_fd(1, "%s%i.%i", color, (time / 1000), mod);
	}

	if (!last) {
		ft_printf_fd(1, "/");
	} else {
		ft_printf_fd(1, " ms\n");
	}
}

void display_clear_summary(t_context *c)
{
	ft_printf_fd(1, FILL_YELLOW"--- %s ping statistics ---\n"RESET, inet_ntoa(*(struct in_addr *)&c->dst_sockaddr.sin_addr.s_addr));
	ft_printf_fd(1, "%u packets transmitted, %u packets received, %u%% packet loss\n", c->summary.nb_send, c->summary.nb_rcv, (c->summary.nb_err * 100) / c->summary.nb_send);
	ft_printf_fd(1, "round-trip "GREEN"min"RESET"/"YELLOW"avg"RESET"/"RED"max"RESET"/"CYAN"stddev"RESET" = ", c->summary.min, c->summary.average, c->summary.max, c->summary.stddev);
	display_ms_time(GREEN, c->summary.min, 0);
	display_ms_time(YELLOW ,c->summary.average, 0);
	display_ms_time(RED, c->summary.max, 0);
	display_ms_time(CYAN, c->summary.stddev, 1);
	// round-trip min/avg/max/stddev = 0.849/1.249/2.561/0.661 ms
}

/**
 *	@brief Main function
 *	@param argc number of arguments
 *	@param argv arguments, destination address and potential flag
*/
int main(int argc, char **argv)
{
    t_context       c;
    int8_t          ret = 1;
    
    if (argc < 2) {
        ft_printf_fd(2, PURPLE"%s: usage error: Destination address required\n"RESET, argv[0]);
        return (1);
    }
    c = init_ping_context(argv[1]);
    if (c.send_sock == -1 || c.rcv_sock == -1) {
        goto free_socket_label;
    } else if (!send_ping(&c)) {
        goto free_socket_label;
    }
    /* If no error occur return code is 0*/
	ret = 0;

    /* Free socket label */
    free_socket_label:
    close_multi_socket(c.rcv_sock, c.send_sock);
	if (c.summary.rcv_time_lst) {
		ft_lstclear(&c.summary.rcv_time_lst, free);
	}
	if (c.summary.nb_send > 0) {
		display_clear_summary(&c);
	}
    ft_printf_fd(1, CYAN"Socket closed return in main\n"RESET);
    
    return (ret);
}