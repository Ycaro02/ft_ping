#include "../include/ft_ping.h"

/**
 * @brief Display IP header details
 * @param header IP header structure
*/
void display_detail_iphdr(struct iphdr *header)
{
    ft_printf_fd(1, FILL_YELLOW"| IP Header |\n"RESET);
    ft_printf_fd(1, "   |-IP Version        : %u\n", (unsigned int)header->version);
    ft_printf_fd(1, "   |-Internet Header Length : %u DWORDS or %u Bytes\n", (unsigned int)header->ihl, ((unsigned int)(header->ihl)) * 4);
    ft_printf_fd(1, "   |-Type Of Service : %u\n", (unsigned int)header->tos);
    ft_printf_fd(1, "   |-Total Length : %u Bytes\n", ntohs(header->tot_len));
    ft_printf_fd(1, "   |-Identification : %u\n", ntohs(header->id));
    ft_printf_fd(1, "   |-Fragment Offset : %u\n", ntohs(header->frag_off));
    ft_printf_fd(1, "   |-Time To Live : %u\n", (unsigned int)header->ttl);
    ft_printf_fd(1, "   |-Protocol : %u\n", (unsigned int)header->protocol);
    ft_printf_fd(1, "   |-Header Checksum : %u\n", ntohs(header->check));
    ft_printf_fd(1, "   |-Source IP : %s\n", inet_ntoa(*(struct in_addr *)&header->saddr));
    ft_printf_fd(1, "   |-Destination IP : %s\n", inet_ntoa(*(struct in_addr *)&header->daddr));
}


char *get_str_msg_type(uint8_t type)
{
	if (type == ICMP_ECHOREPLY) {
		return (YELLOW"ECHO REPLY"RESET);
	} else if (type == ICMP_ECHO) {
		return (YELLOW"ECHO REQUEST"RESET);
	} else if (type == ICMP_DEST_UNREACH) {
		return (RED"DESTINATION UNREACHABLE"RESET);
	} else if (type == ICMP_SOURCE_QUENCH) {
		return (RED"SOURCE QUENCH"RESET);
	} else if (type == ICMP_REDIRECT) {
		return (RED"REDIRECT"RESET);
	} else if (type == ICMP_TIME_EXCEEDED) {
		return (RED"TIME EXCEEDED"RESET);
	} else if (type == ICMP_PARAMETERPROB) {
		return (RED"PARAMETER PROBLEM"RESET);
	} else if (type == ICMP_TIMESTAMP) {
		return (RED"TIMESTAMP"RESET);
	} else if (type == ICMP_TIMESTAMPREPLY) {
		return (RED"TIMESTAMP REPLY"RESET);
	} else if (type == ICMP_INFO_REQUEST) {
		return (RED"INFORMATION REQUEST"RESET);
	} else if (type == ICMP_INFO_REPLY) {
		return (RED"INFORMATION REPLY"RESET);
	} else if (type == ICMP_ADDRESS) {
		return (RED"ADDRESS MASK REQUEST"RESET);
	} else if (type == ICMP_ADDRESSREPLY) {
		return (RED"ADDRESS MASK REPLY"RESET);
	} else {
		return ("UNKNOWN");
	}
}

/**
 * @brief Display ICMP header details
 * @param header ICMP header structure
*/
void display_detail_icmphdr(struct icmphdr *header)
{
	char *icmp_type = get_str_msg_type(header->type);
    ft_printf_fd(1, PURPLE"| ICMP Header |\n"RESET);
    ft_printf_fd(1, "   |-Type : %u %s\n", (unsigned int)(header->type), icmp_type);
    ft_printf_fd(1, "   |-Code : %u\n", (unsigned int)(header->code));
    ft_printf_fd(1, "   |-Checksum : %u\n", ntohs(header->checksum));
    ft_printf_fd(1, "   |-ID : %u\n", ntohs(header->un.echo.id));
    ft_printf_fd(1, "   |-Sequence : %u\n", ntohs(header->un.echo.sequence));
}

/**
 *	@brief Display formated char data from ICMP packet
 *	@param data Pointer to data
 *	@param size Size of data
*/
void display_char_data(uint8_t *data, size_t size)
{
	ft_printf_fd(1, CYAN"| ICMP char Data |\n"RESET);
	for (size_t i = 0; i < size; i++) {
		if (i == 0) {
			ft_printf_fd(1, "-------------------------\n");
		}
		if (i % SHORT_INT_BITS == 0 && i != 0) {
			ft_printf_fd(1, "\t|\n|   ");
		} else if (i == 0) {
			ft_printf_fd(1, "|   ");
		}
		if (ft_isprint(data[i])) {
			ft_printf_fd(1, "%c", data[i]);
		} else {
			ft_printf_fd(1, ".");
		}
	}
	ft_printf_fd(1, "\t\t|\n-------------------------\n");
}

/**
 *	@brief Display brut data from ICMP packet
 *	@param data Pointer to data
 *	@param size Size of data
*/
void display_brut_icmp_data(uint8_t *data, size_t size)
{
	ft_printf_fd(1, CYAN"| ICMP Brut Data |\n"RESET);
	for (size_t i = 0; i < size; i++) {
		if (i % SHORT_INT_BITS == 0) {
			ft_printf_fd(1, "\n   ");
		}
		ft_printf_fd(1, "%u ", (unsigned char)data[i]);
	}
	ft_printf_fd(1, "\n\n");
	display_char_data(data, size);
}

/* @brief Detail display paquet take separate args to be easiest call when receive data buffer*/
void display_detail_packet(t_iphdr *ip_hdr, t_icmphdr *icmp_hdr, uint8_t *data)
{
	display_detail_iphdr(ip_hdr);
	display_detail_icmphdr(icmp_hdr);
	display_brut_icmp_data(data, ICMP_DATA_SIZE);
}


void display_ping_state(t_ping_state *state)
{
	ft_printf_fd(1, YELLOW"| Ping State |\n"RESET);
	ft_printf_fd(1, "   |-Send time : %i\n", state->send_time);
	ft_printf_fd(1, "   |-Receive time : %i\n", state->rcv_time);
}

void display_ping_summary(t_ping_sum *sum)
{
	ft_printf_fd(1, YELLOW"| Ping Summary |\n"RESET);
	ft_printf_fd(1, "   |-Min time : %i\n", sum->min);
	ft_printf_fd(1, "   |-Max time : %i\n", sum->max);
	ft_printf_fd(1, "   |-Average time : %i\n", sum->average);
	ft_printf_fd(1, "   |-Number of send : %u\n", sum->nb_send);
	ft_printf_fd(1, "   |-Number of received : %u\n", sum->nb_rcv);
	ft_printf_fd(1, "   |-Number of error : %u\n", sum->nb_err);
}

// static void display_formated_time(suseconds_t time) {
// 	ft_printf_fd(1, "time="YELLOW"%i.%i ms\n"RESET, (time / 1000), (time % 1000));
// }

static void display_clean_data(t_context *c, t_iphdr *iphdr ,t_icmphdr *icmphdr)
{
	char buff[1024];
	char *dest_str = inet_ntoa(*(struct in_addr *)&(c->dst_sockaddr.sin_addr.s_addr));


	// ft_bzero(buff, 1024);
	// sprintf(buff, "PING "CYAN"%s (%s)"RESET": "GREEN"%d"RESET" data bytes\n", dest_str, dest_str, ICMP_DATA_SIZE);
	// ft_printf_fd(1, "%s", buff);


	ft_bzero(buff, 1024);
	sprintf(buff, GREEN"64"RESET" bytes from "PURPLE"%s"RESET": icmp_seq="ORANGE"%u"RESET" ttl="RED"%d "RESET, dest_str, ntohs(icmphdr->un.echo.sequence), iphdr->ttl);
	ft_printf_fd(1, "%s", buff);

	display_ms_time(YELLOW, c->state.rcv_time - c->state.send_time, TRUE);


	// display_formated_time(c->state.rcv_time - c->state.send_time);
	// display_formated_time(c->summary.average);
	// ft_printf_fd(1, "64 bytes from %s: icmp_seq=%u ttl=%d ", dest_str, ntohs(icmphdr->un.echo.sequence), iphdr->ttl);
}


void display_receive_lst(t_list *rcv_list)
{
	t_list *tmp = rcv_list;
	suseconds_t *rcv_time = NULL;
	int i = 0;

	ft_printf_fd(1, YELLOW"| Receive Time List |\n"RESET);
	while (tmp) {
		rcv_time = (suseconds_t *)tmp->content;
		ft_printf_fd(1, "%i, ", *rcv_time);
		tmp = tmp->next;
		i++;
	}
	ft_printf_fd(1, "\n");
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
	struct sockaddr_in src_addr;
    struct iphdr    *ip_hdr;
    struct icmphdr  *icmp_hdr;
    ssize_t         bytes_received;
	socklen_t addr_len = sizeof(src_addr);


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
