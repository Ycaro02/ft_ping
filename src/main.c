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
    if ((rcv_sock = open_rcv_socket()) == -1 || (send_sock = open_send_socket()) == -1) {
        return (1);
    }
    ft_bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = str_to_addr(argv[1]);
    ft_printf_fd(1, YELLOW"Socket opened fd %d IP address %s\n"RESET, rcv_sock, inet_ntoa(addr.sin_addr));
    if (bind_socket(rcv_sock, &addr) == -1 || bind_socket(send_sock, &addr) == -1) {
        close_socket(rcv_sock);
        return (1);
    }
    
    in_addr_t dest_addr = inet_addr("192.168.1.1");
    t_ping_packet packet = build_ping_packet(addr.sin_addr.s_addr, dest_addr, BRUT_DATA);    
    display_ping_packet(packet);


    errno = 0;
    ssize_t send_ret = sendto(send_sock, &packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr));
    if (send_ret == -1) {
        perror("sendto");
        close_socket(send_sock);
        return (1);
    }
    ft_printf_fd(1, GREEN"Packet sent %u bytes to 192.168.1.1\n"RESET, send_ret);

    ret = listen_icmp_reply(rcv_sock);
    close_socket(rcv_sock);
    close_socket(send_sock);
    ft_printf_fd(1, CYAN"Socket closed return in main\n"RESET);
    return (ret);
}