#include "../include/ft_ping.h"


void display_rcv_timelist(t_list *rcv_list)
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

char *get_str_msg_type(uint8_t type)
{
	switch (type) {
	case ICMP_ECHOREPLY:
		return (YELLOW"ECHO REPLY"RESET);
	case ICMP_ECHO:
		return (YELLOW"ECHO REQUEST"RESET);
	case ICMP_DEST_UNREACH:
		return (RED"DESTINATION UNREACHABLE"RESET);
	case ICMP_SOURCE_QUENCH:
		return (RED"SOURCE QUENCH"RESET);
	case ICMP_REDIRECT:
		return (RED"REDIRECT"RESET);
	case ICMP_TIME_EXCEEDED:
		return (RED"Time to live exceeded"RESET);
	case ICMP_PARAMETERPROB:
		return (RED"PARAMETER PROBLEM"RESET);
	case ICMP_TIMESTAMP:
		return (RED"TIMESTAMP"RESET);
	case ICMP_TIMESTAMPREPLY:
		return (RED"TIMESTAMP REPLY"RESET);
	case ICMP_INFO_REQUEST:
		return (RED"INFORMATION REQUEST"RESET);
	case ICMP_INFO_REPLY:
		return (RED"INFORMATION REPLY"RESET);
	case ICMP_ADDRESS:
		return (RED"ADDRESS MASK REQUEST"RESET);
	case ICMP_ADDRESSREPLY:
		return (RED"ADDRESS MASK REPLY"RESET);
	default:
		return ("UNKNOWN");
	}
	return ("UNKNOWN");
}
