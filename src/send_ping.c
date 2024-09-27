#include "../include/ft_ping.h"

/**
 * @brief signal handler function for SIGNINT
*/
static void signal_handler(int signum)
{
    (void)signum;
    DEBUG_PRINT(YELLOW"\nTimeout SIGINT: %d\n"RESET, signum);
    g_signal_received = 1;
}

/**
 * @brief Initialize signal handler for SIGINT
*/
static int init_signal_handler(void)
{
	if (signal(SIGINT, signal_handler) == SIG_ERR) {
		ft_printf_fd(2, RED"Error: Can't catch SIGINT\n"RESET);
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
        , (struct sockaddr *)&c->dest.sockaddr, sizeof(c->dest.sockaddr));
    if (send_ret == -1) {
        perror("ft_ping: sending packet");
        return (0);
    }
    c->state.send_time = get_ms_time();
    c->summary.nb_send++;
    // display_detail_packet(&packet.iphdr, &packet.icmphdr, packet.data);
    // ft_printf_fd(1, GREEN"Packet sent %u bytes to %s\n"RESET, send_ret, inet_ntoa(*(struct in_addr *)&c->dest.sockaddr.sin_addr.s_addr));
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
    /* update ICMP timestamp */
	gettimeofday((struct timeval *)packet->data , NULL);
    /* Reset checksum */
    packet->icmphdr.checksum = 0;    
    packet->iphdr.check = 0;
    /* Compute ICMP checksum */
    packet->icmphdr.checksum = compute_checksum((uint16_t *)&packet->icmphdr, ICMP_HDR_SIZE + ICMP_DATA_SIZE);
    /* Compute IP checksum */
    packet->iphdr.check = compute_checksum((uint16_t *)&packet->iphdr, IP_HDR_SIZE);

}

/**
 *  @brief Display ping first stat
*/
static void display_first_stat(t_context *c, t_ping_packet packet)
{
    char buff[BUFF_SIZE];
    char *dest_str = inet_ntoa(*(struct in_addr *)&(c->dest.sockaddr.sin_addr.s_addr));
    char *name = c->dest.name ? c->dest.name : dest_str;

    ft_bzero(buff, BUFF_SIZE);
    sprintf(buff, "PING "CYAN"%s (%s)"RESET": "RED"%d"RESET" data bytes", name, dest_str, ICMP_DATA_SIZE);
    ft_printf_fd(1, "%s", buff);
	if (c->flag & V_OPTION) {
		ft_printf_fd(1, ", "PINK"id %p = %d"RESET, ntohs(packet.icmphdr.un.echo.id), ntohs(packet.icmphdr.un.echo.id));
	} 
	ft_printf_fd(1, "\n");
}

/**
 *  @brief Send ping packet and listen for reply
 *  @param c ping context
 *  @return 1 if success 0 if failed
*/
int send_ping(t_context *c)
{
    int8_t          error = 0, listen_bool = 1, count_opt = 0;
	uint8_t			nb_to_send = 1;

	if (has_flag(c->flag, C_OPTION)) {
		count_opt = 1;
		nb_to_send = c->opt_value.count;
	}
    c->packet = build_ping_packet(c, c->src_addr, c->dest.sockaddr.sin_addr.s_addr);
    display_first_stat(c, c->packet);
    while (nb_to_send != 0) {
        listen_bool = g_signal_received == 0;
		// ft_printf_fd(1, "listen_bool %d for addr %s\n", listen_bool, inet_ntoa(*(struct in_addr *)&c->dest.sockaddr.sin_addr.s_addr));
		if (!listen_bool && c->summary.nb_send > 0) { 
			return (1);
		} else if (!send_echo_request(c, c->packet)) {
            return (0);
        }
        while (listen_bool && !listen_icmp_reply(c, &error)) ;
        update_packet(&c->packet);
		if (listen_bool) {
        	usleep(ONE_SEC);
		}
		if (count_opt) {
			--nb_to_send;
		}
    }
    return (1);
}

/**
 * @brief Reset data in context
 * @param c ping context to reset
*/
static void reset_data(t_context *c)
{
    if (c->summary.nb_send > 0) {
        display_clear_summary(c);
    }
    if (c->summary.rcv_time_lst) {
        ft_lstclear(&c->summary.rcv_time_lst, free);
    }
    ft_bzero(&c->summary, sizeof(c->summary));
    ft_bzero(&c->state, sizeof(c->state));
    if (c->dest.name) {
        free(c->dest.name);
        c->dest.name = NULL;
    }
}

/**
 * @brief Send ping loop, loop over all destination address
 * @param c ping context
 * @return 1 if success 0 if failed
 */
int sending_ping_loop(t_context *c)
{
    t_list *args = c->str_args;

    init_signal_handler();

    while (args) {
        if (!get_destination_addr(args->content, (in_addr_t *)&c->dest.sockaddr.sin_addr.s_addr, &c->dest.name)) {
            return (0);
        } else if (!send_ping(c)) {
            return (0);
        }
        args = args->next;
        if (args) {
            reset_data(c);
        }
    }
    return (1);
}