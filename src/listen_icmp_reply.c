#include "../include/ft_ping.h"

static void display_clean_data(t_context *c, t_iphdr *iphdr ,t_icmphdr *icmphdr)
{
	char buff[1024];
	char *dest_str = inet_ntoa(*(struct in_addr *)&(c->dst_sockaddr.sin_addr.s_addr));

	ft_bzero(buff, 1024);
	sprintf(buff, GREEN"64"RESET" bytes from "PURPLE"%s"RESET": icmp_seq="ORANGE"%u"RESET" ttl="RED"%d "RESET, dest_str, ntohs(icmphdr->un.echo.sequence), iphdr->ttl);
	ft_printf_fd(1, "%s", buff);
	display_ms_time(YELLOW, c->state.rcv_time - c->state.send_time, TRUE);
}

/**
 *  @brief Update ping summary with new ping time (min, max and average)
 *  @param sum ping summary structure
 *  @param start packet sent time
 *  @param end packet receive time
*/
void update_ping_summary(t_context *c, suseconds_t start, suseconds_t end)
{
	suseconds_t diff = end - start;
	suseconds_t *rcv_node = NULL;

	if (c->summary.min == 0 || diff < c->summary.min) {
		c->summary.min = diff;
	}
	if (diff > c->summary.max) {
		c->summary.max = diff;
	}
	// ft_printf_fd(1, "BEFORE c->summary.average %i, c->summary.nb_rcv %i, diff %i\n", c->summary.average, c->summary.nb_rcv, diff);
	// ft_printf_fd(1, RED"AFTER c->summary.average %i, c->summary.nb_rcv %i, diff %i\n"RESET, c->summary.average, c->summary.nb_rcv, diff);
	c->summary.nb_rcv++;
	// c->summary.average = ((c->summary.average * c->summary.nb_rcv) + diff) / (c->summary.nb_rcv + 1);

	rcv_node = ft_calloc(1, sizeof(suseconds_t));
	if (rcv_node) {
		*rcv_node = diff;
		ft_lstadd_back(&c->summary.rcv_time_lst, ft_lstnew(rcv_node));		
	}
}

static void display_clean_error(t_context *c, ssize_t bytes_rcv, uint8_t error)
{
	char buff[1024];
	char *error_str = get_str_msg_type(error);
	char *dest_str = inet_ntoa(*(struct in_addr *)&(c->dst_sockaddr.sin_addr.s_addr));

	ft_bzero(buff, 1024);
	sprintf(buff, "from "CYAN"%s (%s)"RESET" %s\n", dest_str, dest_str, error_str);

	ft_printf_fd(1, ""RED"%u"RESET" bytes %s", bytes_rcv, buff);
	// 92 bytes from livebox.home (192.168.1.1):
}

/**
 *  @brief Listen icmp reply
 *  @param c ping context
 *  @return 1 if success 0 if failed
*/
int8_t listen_icmp_reply(t_context *c, int8_t *error)
{
    uint8_t         buffer[BUFFER_SIZE];
	t_sockaddr_in	src_addr;
    t_iphdr			*ip_hdr;
    t_icmphdr		*icmp_hdr;
    ssize_t			bytes_received;
	socklen_t		addr_len = sizeof(t_sockaddr_in);

	ft_bzero(buffer, BUFFER_SIZE);
	ft_bzero(&src_addr, sizeof(src_addr));

	// ft_printf_fd(1, "Listen on addr: %s\n", inet_ntoa(*(struct in_addr *)&c->dst_sockaddr.sin_addr.s_addr));
	errno = 0;
	bytes_received = recvfrom(c->rcv_sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&src_addr, &addr_len);
	// ft_printf_fd(1, "bytes_received %d from %s\n", bytes_received, inet_ntoa(src_addr.sin_addr));
	if (src_addr.sin_addr.s_addr != c->dst_sockaddr.sin_addr.s_addr) {
		return (FALSE);
	} else if (g_signal_received) {
		return (TRUE);
	} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
		ft_printf_fd(2, RED"Timeout Reached\n"RESET);
		return (TRUE);
	} else if (bytes_received < PACKET_SIZE) {
		ft_printf_fd(1, RED"\nNot enought bytes received number: %d\n"RESET, bytes_received);
		perror("recvfrom");
		*error = 0;
		return (TRUE);
	}

	ip_hdr = (struct iphdr *)buffer;
	icmp_hdr = (struct icmphdr *)(buffer + IP_HDR_SIZE);
	if (bytes_received > PACKET_SIZE) { /* care here need to check all case before read data*/
		display_clean_error(c, bytes_received - IP_HDR_SIZE, icmp_hdr->type);
		return (TRUE);
	} 
	if (verify_checksum(buffer, ip_hdr->check, icmp_hdr->checksum) == FALSE) {
		*error = 0;
		return (TRUE);
	}
	c->state.rcv_time = get_ms_time();
	update_ping_summary(c, c->state.send_time, c->state.rcv_time);
	display_clean_data(c, ip_hdr, icmp_hdr);
	ft_bzero(buffer, BUFFER_SIZE);
	return (TRUE);
}
