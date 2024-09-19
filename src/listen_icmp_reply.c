#include "../include/ft_ping.h"

static void display_clean_data(t_context *c, t_iphdr *iphdr ,t_icmphdr *icmphdr)
{
	char buff[BUFF_SIZE];
	char *dest_str = inet_ntoa(*(struct in_addr *)&(c->dest.sockaddr.sin_addr.s_addr));

	ft_bzero(buff, BUFF_SIZE);
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


static void display_ip_header_dump(t_iphdr *iphdr, t_icmphdr *icmphdr)
{
	ft_printf_fd(1, "IP Hdr Dump:\n");
	for (int i = 0; i < 20; i += 2) {
		dprintf(1, "%02x%02x ", ((uint8_t *)iphdr)[i], ((uint8_t *)iphdr)[i + 1]);
	}
	ft_printf_fd(1, "\nVr HL TOS  Len   ID Flg  off TTL Pro  cks      Src	Dst	Data\n");
	/*			Vr	 HL  TOS  LEN  ID   flg off   ttl   pro  cks */
	dprintf(1, "%2d  %2d %02x %04x %04x %3d %04x  %02x  %02x %04x ",
		iphdr->version, iphdr->ihl, iphdr->tos, ntohs(iphdr->tot_len), ntohs(iphdr->id)\
		, 0,  ntohs(iphdr->frag_off), iphdr->ttl, iphdr->protocol, ntohs(iphdr->check));
	
	ft_printf_fd(1, "%s  ", inet_ntoa(*(struct in_addr *)&iphdr->saddr));
	ft_printf_fd(1, "%s\n", inet_ntoa(*(struct in_addr *)&iphdr->daddr));
	
	dprintf(1, "ICMP: type %u, code %u, size %lu, id 0x%04x, seq 0x%04x\n", icmphdr->type\
		, icmphdr->code, ntohs(iphdr->tot_len) - IP_HDR_SIZE\
		, ntohs(icmphdr->un.echo.id), ntohs(icmphdr->un.echo.sequence));
}

/**
 * To addd
 *	PING google.com (142.250.75.238): 56 data bytes, id 0x389f = 14495
 *	92 bytes from 80.10.239.17: Time to live exceeded
 ** IP Hdr Dump:
 *	4500 0054 35d5 4000 0101 a731 c0a8 0112 8efa 4bee 
 *	Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src	Dst	Data
 *	4  5  00 0054 35d5   2 0000  01  01 a731 192.168.1.18  142.250.75.238 
 **	ICMP: type 8, code 0, size 64, id 0x389f, seq 0x0000
*/
static void display_clean_error(struct in_addr *src_addr, ssize_t bytes_rcv, uint8_t error)
{
	char buff[BUFF_SIZE];
	char *error_str = get_str_msg_type(error);
	// char *dest_str = inet_ntoa(*(struct in_addr *)&(c->dest.sockaddr.sin_addr.s_addr));
	char *dest_str = inet_ntoa(*src_addr);

	ft_bzero(buff, BUFF_SIZE);
	sprintf(buff, "from "CYAN"%s (%s)"RESET" %s\n", dest_str, dest_str, error_str);

	ft_printf_fd(1, ""RED"%u"RESET" bytes %s", bytes_rcv, buff);

	// 92 bytes from livebox.home (192.168.1.1):
	// if -v display ip header dump
}


static int8_t is_timeout_time(suseconds_t start, suseconds_t timeout, uint8_t flag)
{
	if (!has_flag(flag, TIMEOUT_OPTION)) {
		return (FALSE);
	}
	return (get_ms_time() >= (start + (timeout * ONE_SEC)));
}

/**
 *	@brief Parse icmp reply
 *	@param c ping context structure
 *	@param buffer buffer to store icmp reply
 *	@param error pointer to set at 1 if error occur
 *	@param ip_header_id ip header id to check if icmp reply is for the current packet
 *	@return STOP if error occur, CONTINUE if we need to continue to listen icmp reply, CORRECT_BUFFER if icmp reply is correct
*/
static int8_t parse_icmp_reply(t_context *c, uint8_t buffer[], int8_t *error)
{
    ssize_t			bytes_received;
    t_iphdr			*ip_hdr = NULL;
	t_sockaddr_in	src_addr;
	socklen_t		addr_len = sizeof(t_sockaddr_in);

	ft_bzero(&src_addr, sizeof(src_addr));
	errno = 0;
	bytes_received = recvfrom(c->rcv_sock, buffer, BUFF_SIZE, 0, (struct sockaddr*)&src_addr, &addr_len);
	
	int8_t timeout_down = is_timeout_time(c->start, c->opt_value.timeout, c->flag);
	if (timeout_down) {
		// ft_printf_fd(1, "Timeout need to leave\n");
		g_signal_received = 1;
	}
	
	if (g_signal_received) { /* signal receive case SIGINT */
		return (STOP_LISTEN);
	} else if (errno == EAGAIN || errno == EWOULDBLOCK) { /* timeout/linger case */
		ft_printf_fd(2, RED"Timeout Reached\n"RESET);
		return (STOP_LISTEN);
	} else if (bytes_received < PACKET_SIZE) { /* not enought bytes received or recvfrom error (-1 returned) */
		ft_printf_fd(1, RED"\nNot enought bytes received number: %d\n"RESET, bytes_received);
		perror("recvfrom");
		*error = 1;
		return (STOP_LISTEN);
	} else if (bytes_received > PACKET_SIZE) { /* error case */
		ip_hdr = (t_iphdr *)(buffer + IP_HDR_SIZE + ICMP_HDR_SIZE); /* get sent ip header: first ip_hdr_size of data */
		if (ip_hdr->id != c->packet.iphdr.id) {
			#ifdef DEBUG
				ft_printf_fd(1, RED"ID mismatch %u != %u\n"RESET, ntohs(ip_hdr->id), ntohs(c->packet.iphdr.id));
			#endif
			return (CONTINUE_LISTEN);
		}
		display_clean_error((struct in_addr *)&src_addr.sin_addr.s_addr, bytes_received - IP_HDR_SIZE, ((t_icmphdr *)(buffer + IP_HDR_SIZE))->type);
		if (has_flag(c->flag, V_OPTION)) {
			display_ip_header_dump(ip_hdr, (t_icmphdr *)((void *)ip_hdr + IP_HDR_SIZE));
		}
		*error = 1;
		return (STOP_LISTEN);
	}

	ip_hdr = (t_iphdr *)buffer;
	t_icmphdr *icmp_hdr = (t_icmphdr *)(buffer + IP_HDR_SIZE);
	if (ip_hdr->protocol == IPPROTO_ICMP && icmp_hdr->type != ICMP_ECHO && icmp_hdr->un.echo.id == c->packet.icmphdr.un.echo.id) {
		return (CORRECT_BUFFER);
	}

	return (CONTINUE_LISTEN);
}

/**
 *  @brief Listen icmp reply
 *  @param c ping context structure
 *	@param error pointer to set at 1 if error occur (no enought bytes or invalid checksum)
 *  @return CONTINUE if message receive from source address and need to go next send\listen, STOP if we need to continue to listen same reply
*/
int8_t listen_icmp_reply(t_context *c, int8_t *error)
{
    uint8_t         buffer[BUFF_SIZE];
    t_iphdr			*ip_hdr;
    t_icmphdr		*icmp_hdr;
	int8_t			ret;


	ft_bzero(buffer, BUFF_SIZE);

	ret = parse_icmp_reply(c, buffer, error);
	if (ret != CORRECT_BUFFER) {
		// ft_printf_fd(1, "Incorect ret %d\n", ret);
		return (ret);
	}

	ip_hdr = (t_iphdr *)buffer;
	icmp_hdr = (t_icmphdr *)(buffer + IP_HDR_SIZE);

	if (ntohs(icmp_hdr->un.echo.id) != ntohs(c->packet.icmphdr.un.echo.id)) {
		#ifdef DEBUG
			ft_printf_fd(1, RED"ID mismatch %u != %u\n"RESET, ntohs(ip_hdr->id), ntohs(c->packet.icmphdr.un.echo.id));
		#endif
		return (CONTINUE_LISTEN);
	}
	
	if (verify_checksum(buffer, ip_hdr->check, icmp_hdr->checksum) == FALSE) {
		*error = 1;
		return (STOP_LISTEN);
	}

	// display_detail_packet(ip_hdr, icmp_hdr, buffer + IP_HDR_SIZE + ICMP_HDR_SIZE);

	c->state.rcv_time = get_ms_time();
	update_ping_summary(c, c->state.send_time, c->state.rcv_time);
	display_clean_data(c, ip_hdr, icmp_hdr);
	ft_bzero(buffer, BUFF_SIZE);
	return (STOP_LISTEN);
}
