#include "../include/ft_ping.h"


/**
 * @brief signal handler function for SIGNINT
*/
static void signal_handler(int signum)
{
    (void)signum;
    ft_printf_fd(2, RED"\nSIGINT Catch\n"RESET);
    g_signal_received = 1;
}

/**
 * @brief Initialize signal handler for SIGINT
*/
static int init_signal_handler(void)
{
	if (signal(SIGINT, signal_handler) == SIG_ERR) {
			ft_printf_fd(2, "Can't catch SIGINT\n");
			return (-1);
	}
	return (0);
}

uint8_t send_echo_request(t_context *c, t_ping_packet packet)
{
    ssize_t         send_ret = sendto(c->send_sock, &packet, sizeof(packet), 0, (struct sockaddr *)&c->dst_sockaddr, sizeof(c->dst_sockaddr));
    
    errno = 0;
    if (send_ret == -1) {
        perror("sendto");
        return (0);
    }
    c->state.send_time = get_ms_time();
    c->summary.nb_send++;
    // display_detail_packet(&packet.iphdr, &packet.icmphdr, packet.data);
    // ft_printf_fd(1, GREEN"Packet sent %u bytes to %s\n"RESET, send_ret, inet_ntoa(*(struct in_addr *)&c->dst_sockaddr.sin_addr.s_addr));
    return (1);
}

void increment_icmp_seq(uint16_t *seq_id)
{
    uint16_t seq = ntohs(*seq_id);
    seq++;
    *seq_id = htons(seq);
}

void update_packet(t_ping_packet *packet)
{
    increment_icmp_seq(&packet->icmphdr.un.echo.sequence);
    /* Reset checksum */
    packet->icmphdr.checksum = 0;    
    packet->iphdr.check = 0;
    /* Compute ICMP checksum */
    packet->icmphdr.checksum = compute_checksum((uint16_t *)&packet->icmphdr, ICMP_HDR_SIZE + ICMP_DATA_SIZE);
    /* Compute IP checksum */
    packet->iphdr.check = compute_checksum((uint16_t *)&packet->iphdr, IP_HDR_SIZE);
}

int send_ping(t_context *c)
{
    t_ping_packet   packet;
    init_signal_handler();

    packet = build_ping_packet(c->src_addr, c->dst_sockaddr.sin_addr.s_addr);
    while (!g_signal_received && c->summary.nb_send < 5) {
        init_signal_handler();
        if (!send_echo_request(c, packet)) {
            return (0);
        }
        listen_icmp_reply(c);
        update_packet(&packet);
        usleep(1000000);
    }
    return (1);
}
