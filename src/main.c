#include "../include/ft_ping.h"

int g_signal_received = 0;

/**
 *	@brief Initialize ping context
 *	@param dest_addr destination address to send ping
*/
t_context init_ping_context(char *dest_addr)
{
    t_context c;

    ft_bzero(&c, sizeof(c));
    c.src_addr = get_process_ipv4_addr();
    c.dst_sockaddr.sin_family = AF_INET;
    c.dst_sockaddr.sin_addr.s_addr = ipv4_str_toaddr(dest_addr);
    c.send_sock = open_send_socket();
    c.rcv_sock = open_rcv_socket();

    return (c);    
}

uint8_t send_echo_request(t_context *c)
{
    t_ping_packet   packet = build_ping_packet(c->src_addr, c->dst_sockaddr.sin_addr.s_addr);
    ssize_t         send_ret = sendto(c->send_sock, &packet, sizeof(packet), 0, (struct sockaddr *)&c->dst_sockaddr, sizeof(c->dst_sockaddr));
    
    errno = 0;
    if (send_ret == -1) {
        perror("sendto");
        return (0);
    }
    c->state.send_time = get_ms_time();
    c->summary.nb_send++;
    ft_printf_fd(1, GREEN"Packet sent %u bytes to %s\n"RESET, send_ret, inet_ntoa(*(struct in_addr *)&c->dst_sockaddr.sin_addr.s_addr));
    return (1);
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
        goto free_socket;
    }
    /* Send first packet */
    if (!send_echo_request(&c)) {
        goto free_socket;
    }

    ret = listen_icmp_reply(&c);

    /* Free socket label */
    free_socket:
    close_multi_socket(c.rcv_sock, c.send_sock);
    ft_printf_fd(1, CYAN"Socket closed return in main\n"RESET);
    
    return (ret);
}