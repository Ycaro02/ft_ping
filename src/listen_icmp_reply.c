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

/**
 * @brief Display ICMP header details
 * @param header ICMP header structure
*/
void display_detail_icmphdr(struct icmphdr *header)
{
	char *icmp_type;
	if (header->type == ICMP_ECHOREPLY) {
		icmp_type = YELLOW"ECHO REPLY"RESET;
	} else if (header->type == ICMP_ECHO) {
		icmp_type = CYAN"ECHO REQUEST"RESET;
	} else {
		icmp_type = RED"UNKNOWN"RESET;
	}
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
static void display_detail_packet(t_iphdr *ip_hdr, t_icmphdr *icmp_hdr, uint8_t *data)
{
	display_detail_iphdr(ip_hdr);
	display_detail_icmphdr(icmp_hdr);
	display_brut_icmp_data(data, ICMP_DATA_SIZE);
}


static void display_ping_state(t_ping_state *state)
{
	printf(YELLOW"| Ping State |\n"RESET);
	printf("   |-Start time : %ld\n", state->start);
	printf("   |-Send time : %ld\n", state->send_time);
	printf("   |-Min time : %ld\n", state->min);
	printf("   |-Max time : %ld\n", state->max);
	printf("   |-Average time : %ld\n", state->average);
	printf("   |-Number of send : %u\n", state->nb_send);
	printf("   |-Number of received : %u\n", state->nb_rcv);
	printf("   |-Number of error : %u\n", state->nb_err);
}

int8_t listen_icmp_reply(t_context *c)
{
    uint8_t         buffer[BUFFER_SIZE];
    struct iphdr    *ip_hdr;
    struct icmphdr  *icmp_hdr;
    ssize_t         bytes_received;

    init_signal_handler();

    while (!g_signal_received) {
        // ft_printf_fd(1, YELLOW"Waiting for ICMP Echo Reply, global: %d\n"RESET, g_signal_received);
        errno = 0;
        bytes_received = recvfrom(c->rcv_sock, buffer, BUFFER_SIZE, 0, NULL, NULL);
		if (g_signal_received) {
			break ;
		} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
			ft_printf_fd(2, RED"Timeout Reached\n"RESET);
			continue ;
		} else if (bytes_received != PACKET_SIZE) {
			ft_printf_fd(1, RED"\nWrong bytes received number: %d\n"RESET, bytes_received);
            continue ;
		}
        ip_hdr = (struct iphdr *)buffer;
        icmp_hdr = (struct icmphdr *)(buffer + IP_HDR_SIZE);
		// ft_printf_fd(1, "buff addr %p, new addr %p, compute %p\n", buffer, icmp_hdr);
     	display_detail_packet(ip_hdr, icmp_hdr, buffer + IP_HDR_SIZE + ICMP_HDR_SIZE);
		if (verify_checksum(buffer, ip_hdr->check, icmp_hdr->checksum) == FALSE) {
			return (1);
		}
		suseconds_t rcv_time = get_ms_time();
		update_ping_state(&c->state, c->state.send_time, rcv_time);
		display_ping_state(&c->state);
		ft_bzero(buffer, BUFFER_SIZE);
    }
	return (0);
}


//   ft_printf_fd(1, "   |size IP hdr   : %u\n", IP_HDR_SIZE);
// ft_printf_fd(1, "   |size ICMP hdr : %u\n", ICMP_HDR_SIZE);
// ft_printf_fd(1, "   |size data     : %u\n", ICMP_DATA_SIZE);