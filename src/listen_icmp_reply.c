#include "../include/ft_ping.h"

static void signal_handler(int signum)
{
    (void)signum;
    ft_printf_fd(2, RED"\nSIGINT Catch\n"RESET);
    g_signal_received = 1;
}

static int init_signal_handler(void)
{
	if (signal(SIGINT, signal_handler) == SIG_ERR) {
			ft_printf_fd(2, "Can't catch SIGINT\n");
			return (-1);
	}
	return (0);
}

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

void display_detail_icmphdr(struct icmphdr *header)
{
    ft_printf_fd(1, PURPLE"| ICMP Header |\n"RESET);
    ft_printf_fd(1, "   |-Type : %u\n", (unsigned int)(header->type));
    ft_printf_fd(1, "   |-Code : %u\n", (unsigned int)(header->code));
    ft_printf_fd(1, "   |-Checksum : %u\n", ntohs(header->checksum));
    ft_printf_fd(1, "   |-ID : %u\n", ntohs(header->un.echo.id));
    ft_printf_fd(1, "   |-Sequence : %u\n", ntohs(header->un.echo.sequence));
}

void display_char_data(char *data, size_t size)
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

void display_icmp_data(char *data, size_t size)
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

void display_ping_packet(t_ping_packet packet)
{
    display_detail_iphdr(&packet.iphdr);
    display_detail_icmphdr(&packet.icmphdr);
    display_icmp_data(packet.data, ICMP_DATA_SIZE);
}

int8_t listen_icmp_reply(int sock)
{
    char            buffer[1024];
    ssize_t         bytes_received;
    struct iphdr    *ip_hdr;
    struct icmphdr  *icmp_hdr;
	int mult = 1;

    init_signal_handler();

    while (!g_signal_received) {
        // ft_printf_fd(1, YELLOW"Waiting for ICMP Echo Reply, global: %d\n"RESET, g_signal_received);
        errno = 0;
        bytes_received = recvfrom(sock, buffer, 1024, MSG_DONTWAIT, NULL, NULL);
        if (bytes_received < 0) {
            /* No data continue wait */
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            } else {
                perror("recvfrom");
				return (1);
            }
        }
        ip_hdr = (struct iphdr *)buffer;
        display_detail_iphdr(ip_hdr);
        if (bytes_received != PACKET_SIZE) {
            ft_printf_fd(1, RED"\nWrong bytes received number: %d\n"RESET, bytes_received);
            // continue;
			if (bytes_received > PACKET_SIZE) {
        		mult = 2;
			}
		}
        icmp_hdr = (struct icmphdr *)(buffer + (IP_HDR_SIZE * mult));
		ft_printf_fd(1, "buff addr %p, new addr %p, compute %p\n", buffer, icmp_hdr, buffer + 40);
        display_detail_icmphdr(icmp_hdr);
        if (icmp_hdr->type == ICMP_ECHOREPLY) {
            ft_printf_fd(1, GREEN"\nEcho Reply %u bytes received\n"RESET, bytes_received);
        } else if (icmp_hdr->type == ICMP_ECHO) {
			ft_printf_fd(1, GREEN"\nICMP Echo Request %u bytes received\n"RESET, bytes_received);
		} else {
			ft_printf_fd(1, RED"Received ICMP type %u code %u %u bytes received\n"RESET, icmp_hdr->type, icmp_hdr->code, bytes_received);
		}
        ft_printf_fd(1, "   |size IP hdr   : %u\n", IP_HDR_SIZE);
        ft_printf_fd(1, "   |size ICMP hdr : %u\n", ICMP_HDR_SIZE);
        ft_printf_fd(1, "   |size data     : %u\n", ICMP_DATA_SIZE);
		display_icmp_data(buffer + IP_HDR_SIZE + ICMP_HDR_SIZE, ICMP_DATA_SIZE);
		if (verify_checksum(buffer, ip_hdr->check, icmp_hdr->checksum) == 0) {
			return (1);
		}
		ft_bzero(buffer, 1024);
    }
	return (0);
}


