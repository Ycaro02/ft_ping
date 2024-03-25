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

	// t_icmphdr hdr = build_icmp_hdr();
	// ft_printf_fd(1, "ICMP header type %d code %d checksum %d id %d sequence %d\n", hdr.type, hdr.code, hdr.checksum, hdr.un.echo.id, hdr.un.echo.sequence);
    listen_icmp_reply(sock);
    close_socket(sock);
    ft_printf_fd(1, YELLOW"Socket closed\n"RESET);
    return (0);
}