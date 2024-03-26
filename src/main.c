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


struct sockaddr	uint32_tosockaddr(uint32_t ip)
{
	struct sockaddr_in	addr;

	ft_bzero(&addr, sizeof(struct sockaddr_in));
	addr.sin_addr.s_addr = ip;
	return (*((struct sockaddr *)&addr));
}


int main(int argc, char **argv)
{
    t_sockaddr_in   addr;
    int             rcv_sock;
    int             send_sock;
    int8_t          ret = 0;

    if (argc < 2) {
        ft_printf_fd(2, PURPLE"%s: usage error: Destination address required\n"RESET, argv[0]);
        return (1);
    }

	in_addr_t args_addr = str_to_addr(argv[1]);
    in_addr_t dest_addr = str_to_addr("192.168.1.1");

    if ((rcv_sock = open_rcv_socket()) == -1 || (send_sock = open_send_socket()) == -1) {
        return (1);
    }
    
    ft_bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = dest_addr;
   

    ft_printf_fd(1, YELLOW"Raw Socket open\n"RESET);
    ft_printf_fd(1, YELLOW"Args addr %s\n"RESET, inet_ntoa(*(struct in_addr *)&args_addr));
	ft_printf_fd(1, YELLOW"Dest addr %s\n"RESET, inet_ntoa(*(struct in_addr *)&dest_addr));

    char brut_data[] = {
		109, 243, 2, 102, 0, 0, 0, 0, 54, 209, 12, 0, 0, 0, 0, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
		32, 33, 34, 35, 36, 37, 38, 39
	};
    t_ping_packet packet = build_ping_packet(args_addr, dest_addr, brut_data);    
    // display_ping_packet(packet);

    errno = 0;
    ssize_t send_ret = sendto(send_sock, &packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr));
    if (send_ret == -1) {
        perror("sendto");
        close_socket(rcv_sock);
        close_socket(send_sock);
        return (1);
    }
    ft_printf_fd(1, GREEN"Packet sent %u bytes to %s\n"RESET, send_ret, inet_ntoa(*(struct in_addr *)&dest_addr));

    ret = listen_icmp_reply(rcv_sock);
    close_socket(rcv_sock);
    close_socket(send_sock);
    ft_printf_fd(1, CYAN"Socket closed return in main\n"RESET);
    return (ret);
}