#include "../include/ft_ping.h"

int g_signal_received = 0;

/* Ip address string format to bin format */
in_addr_t str_to_addr(char *str)
{
    struct in_addr addr;

    /* Convert presentation format to binary network format */
    if (inet_pton(AF_INET, str, &addr) <= 0) {
        ft_printf_fd(2, RED"ft_ping: %s: Name or service not known\n"RESET, str);
        exit(1);
    }
    printf("Adresse IP binaire : %08x\n", addr.s_addr);
    return (addr.s_addr);
}

/* Get icmp paquets id */
static uint16_t get_icmp_id()
{
	static uint16_t id = 10000;

	if (id < UINT16_MAX) {
		id++;
	} else {
		id = 0;
	}
	return (id);
}

/* Get icmp paquets sequence id */
static uint16_t get_icmp_id_seq()
{
	static uint16_t id = 1;

	if (id < UINT16_MAX) {
		id++;
	} else {
		id = 0;
	}
	return (id);
}

/* Build icmp header */
t_icmphdr build_icmp_hdr()
{
	t_icmphdr hdr;

	hdr.type = ICMP_ECHO;
	hdr.code = 0;
	hdr.checksum = 0;
	hdr.un.echo.id = get_icmp_id();
	hdr.un.echo.sequence = get_icmp_id_seq();
	return (hdr);
}


static void signal_handler(int signum)
{
    (void)signum;
    ft_printf_fd(2, RED"\nSIGINT Catch: %d\n"RESET, signum);
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


void display_iphdr(struct iphdr *header)
{
    ft_printf_fd(1, YELLOW"IP Header\n"RESET);
    ft_printf_fd(1, "   |-Version : %u\n", (unsigned int)header->version);
    ft_printf_fd(1, "   |-Internet Header Length : %u DWORDS or %u Bytes\n", (unsigned int)header->ihl, ((unsigned int)(header->ihl)) * 4);
    ft_printf_fd(1, "   |-Type Of Service : %u\n", (unsigned int)header->tos);
    ft_printf_fd(1, "   |-Total Length : %u Bytes\n", ntohs(header->tot_len));
    ft_printf_fd(1, "   |-Identification : %u\n", ntohs(header->id));
    ft_printf_fd(1, "   |-Time To Live : %u\n", (unsigned int)header->ttl);
    ft_printf_fd(1, "   |-Protocol : %u\n", (unsigned int)header->protocol);
    ft_printf_fd(1, "   |-Header Checksum : %u\n", ntohs(header->check));
    ft_printf_fd(1, "   |-Source IP : %s\n", inet_ntoa(*(struct in_addr *)&header->saddr));
    ft_printf_fd(1, "   |-Destination IP : %s\n", inet_ntoa(*(struct in_addr *)&header->daddr));
}

void display_icmphdr(struct icmphdr *header)
{
    ft_printf_fd(1, PURPLE"ICMP Header\n"RESET);
    ft_printf_fd(1, "   |-Type : %u\n", (unsigned int)(header->type));
    ft_printf_fd(1, "   |-Code : %u\n", (unsigned int)(header->code));
    ft_printf_fd(1, "   |-Checksum : %u\n", ntohs(header->checksum));
    ft_printf_fd(1, "   |-ID : %u\n", ntohs(header->un.echo.id));
    ft_printf_fd(1, "   |-Sequence : %u\n", ntohs(header->un.echo.sequence));
}

void listen_icmp_reply(int sock)
{
    char            buffer[1024];
    ssize_t         bytes_received;
    struct iphdr    *ip_hdr;
    struct icmphdr  *icmp_hdr;

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
                close(sock);
                exit(EXIT_FAILURE);
            }
        }
        ip_hdr = (struct iphdr *)buffer;
        display_iphdr(ip_hdr);
        icmp_hdr = (struct icmphdr *)(buffer + sizeof(struct iphdr));
        display_icmphdr(icmp_hdr);
        if (icmp_hdr->type == ICMP_ECHOREPLY) {
            ft_printf_fd(1, GREEN"Received ICMP Echo Reply\n"RESET);
        }
    }
}

int main(int argc, char **argv)
{
    t_sockaddr_in   addr;
    int             sock;

    if (argc < 2) {
        ft_printf_fd(2, PURPLE"%s: usage error: Destination address required\n"RESET, argv[0]);
        return (1);
    }
    sock  = open_socket();
    if (sock == -1) {
        return (1);
    }
    ft_bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = str_to_addr(argv[1]);
    ft_printf_fd(1, YELLOW"Socket opened fd %d IP address %s\n"RESET, sock, inet_ntoa(addr.sin_addr));
    if (bind_socket(sock, &addr) == -1) {
        close_socket(sock);
        return (1);
    }

	t_icmphdr hdr = build_icmp_hdr();
	ft_printf_fd(1, "ICMP header type %d code %d checksum %d id %d sequence %d\n", hdr.type, hdr.code, hdr.checksum, hdr.un.echo.id, hdr.un.echo.sequence);
    listen_icmp_reply(sock);
    close_socket(sock);
    ft_printf_fd(1, YELLOW"Socket closed\n"RESET);
    return (0);
}