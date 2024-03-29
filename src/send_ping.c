#include "../include/ft_ping.h"


/**
 * @brief signal handler function for SIGNINT
*/
static void signal_handler(int signum)
{
    (void)signum;
    ft_printf_fd(2, YELLOW"\nTimeout SIGINT\n"RESET);
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

/**
 *  @brief Send encho request call to sendto
 *  @param c ping context
 *  @param packet ping packet to send
 *  @return 1 if success 0 if failed
*/
uint8_t send_echo_request(t_context *c, t_ping_packet packet)
{
    ssize_t         send_ret = 0;
    
    errno = 0;

    send_ret = sendto(c->send_sock, &packet, sizeof(packet), 0\
        , (struct sockaddr *)&c->dst_sockaddr, sizeof(c->dst_sockaddr));
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

/**
 *  @brief Increment icmp header sequence field
 *  @param seq_id pointer to sequence field
*/
void increment_icmp_seq(uint16_t *seq_id)
{
    uint16_t seq = ntohs(*seq_id);
    seq++;
    *seq_id = htons(seq);
}

/**
 *  @brief Update ping packet, increment sequence and recompute checksum
 *  @param packet ping packet to update
*/
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

/**
 *  @brief Send ping packet and listen for reply
 *  @param c ping context
 *  @return 1 if success 0 if failed
*/
int send_ping(t_context *c)
{
    char buff[1024];
    t_ping_packet   packet;
	char *dest_str = inet_ntoa(*(struct in_addr *)&(c->dst_sockaddr.sin_addr.s_addr));
	char *name = c->name ? c->name : dest_str;
    int ret = 1;

    init_signal_handler();

    packet = build_ping_packet(c->src_addr, c->dst_sockaddr.sin_addr.s_addr);

	ft_bzero(buff, 1024);
	sprintf(buff, "PING "CYAN"%s (%s)"RESET": "RED"%d"RESET" data bytes\n", name, dest_str, ICMP_DATA_SIZE);
	ft_printf_fd(1, "%s", buff);
    
    
    while (!g_signal_received && c->summary.nb_send < 5) {
        init_signal_handler();
        if (!send_echo_request(c, packet)) {
            return (0);
        }
        ret = listen_icmp_reply(c);
        update_packet(&packet);
        usleep(1000000); /* possible option -i set by user */
    }
    return (ret);
}
