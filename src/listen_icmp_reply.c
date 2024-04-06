#include "../include/ft_ping.h"

static void display_clean_data(t_context *c, t_iphdr *iphdr ,t_icmphdr *icmphdr)
{
	char buff[1024];
	char *dest_str = inet_ntoa(*(struct in_addr *)&(c->dest.sockaddr.sin_addr.s_addr));

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
	char buff[1024];
	char *error_str = get_str_msg_type(error);
	// char *dest_str = inet_ntoa(*(struct in_addr *)&(c->dest.sockaddr.sin_addr.s_addr));
	char *dest_str = inet_ntoa(*src_addr);

	ft_bzero(buff, 1024);
	sprintf(buff, "from "CYAN"%s (%s)"RESET" %s\n", dest_str, dest_str, error_str);

	ft_printf_fd(1, ""RED"%u"RESET" bytes %s", bytes_rcv, buff);
	// 92 bytes from livebox.home (192.168.1.1):
}

/**
 *  @brief Listen icmp reply
 *  @param c ping context
 *	@param error pointer to set at 1 if error occur (no enought bytes or invalid checksum)
 *  @return True if message receive from source address and need to go next send\listen, FALSE if we need to continue to listen same reply
*/
int8_t listen_icmp_reply(t_context *c, int8_t *error, uint16_t ip_header_id)
{
    uint8_t         buffer[BUFFER_SIZE];
	t_sockaddr_in	src_addr;
    t_iphdr			*ip_hdr;
    t_icmphdr		*icmp_hdr;
    ssize_t			bytes_received;
	socklen_t		addr_len = sizeof(t_sockaddr_in);

	ft_bzero(buffer, BUFFER_SIZE);
	ft_bzero(&src_addr, sizeof(src_addr));

	// ft_printf_fd(1, "Listen on addr: %s\n", inet_ntoa(*(struct in_addr *)&c->dest.sockaddr.sin_addr.s_addr));
	errno = 0;
	bytes_received = recvfrom(c->rcv_sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&src_addr, &addr_len);
	// ft_printf_fd(1, "bytes_received %d from %s\n", bytes_received, inet_ntoa(src_addr.sin_addr));
	if (src_addr.sin_addr.s_addr != c->dest.sockaddr.sin_addr.s_addr)  {
		// ft_printf_fd(1, RED"WRONG ADDR RECEIVE\n"RESET);
		if (bytes_received >= PACKET_SIZE) {
			icmp_hdr = (t_icmphdr *)(buffer + IP_HDR_SIZE);
			ip_hdr = (t_iphdr *)(buffer + IP_HDR_SIZE + ICMP_HDR_SIZE); /* hardcode suppose ip header dont have any option */
			// ft_printf_fd(1, "new header ip ID: %d\n", ntohs(((t_iphdr *)buffer)->id));
			// ft_printf_fd(1, "real header ip ID: %d\n", ntohs(ip_hdr->id));
			if (ntohs(ip_hdr->id) != ip_header_id) {
				return (FALSE);
			}
			/* for -v (mandatory) need to add special display on error, hexadump of ip hdr and display basic info on icmp for*/
			display_clean_error((struct in_addr *)&src_addr.sin_addr.s_addr, bytes_received - IP_HDR_SIZE, icmp_hdr->type);
			*error = 1;
			return (TRUE);
		}
		return (FALSE);
	} else if (g_signal_received) {
		return (TRUE);
	} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
		ft_printf_fd(2, RED"Timeout Reached\n"RESET);
		return (TRUE);
	} else if (bytes_received < PACKET_SIZE) {
		ft_printf_fd(1, RED"\nNot enought bytes received number: %d\n"RESET, bytes_received);
		perror("recvfrom");
		*error = 1;
		return (TRUE);
	}

	ip_hdr = (t_iphdr *)buffer;
	icmp_hdr = (t_icmphdr *)(buffer + IP_HDR_SIZE);
	if (verify_checksum(buffer, ip_hdr->check, icmp_hdr->checksum) == FALSE) {
		*error = 1;
		return (TRUE);
	}
	c->state.rcv_time = get_ms_time();
	update_ping_summary(c, c->state.send_time, c->state.rcv_time);
	display_clean_data(c, ip_hdr, icmp_hdr);
	ft_bzero(buffer, BUFFER_SIZE);
	return (TRUE);
}
