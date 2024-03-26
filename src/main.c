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
    return (addr.s_addr);
}

void close_multi_socket(int sock1, int sock2)
{
    if (sock1 != -1) {
        close_socket(sock1);
    }
    if (sock2 != -1) {
        close_socket(sock2);
    }
}

#include <ifaddrs.h>

static in_addr_t get_process_ipv4_addr()
{
    struct ifaddrs *ifaddr, *ifa;
    in_addr_t addr = 0;
    in_addr_t local_host = str_to_addr(LOCAL_HOST_ADDR);

    errno = 0;
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return (0);
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            addr = sa->sin_addr.s_addr;
            if (addr != local_host) {
                ft_printf_fd(1, CYAN"Process addr %s\n"RESET, inet_ntoa(*(struct in_addr *)&addr));
                break ;
            }
        }
    }
    freeifaddrs(ifaddr);
    return (addr);

}

int main(int argc, char **argv)
{
    t_sockaddr_in   addr;
    int             rcv_sock = -1;
    int             send_sock = -1;
    int8_t          ret = 1;

    in_addr_t       addr_from = get_process_ipv4_addr();

    if (argc < 2) {
        ft_printf_fd(2, PURPLE"%s: usage error: Destination address required\n"RESET, argv[0]);
        return (1);
    }

    in_addr_t dest_addr = str_to_addr(argv[1]);
    ft_bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = dest_addr;
    ft_printf_fd(1, YELLOW"Source addr %s\n"RESET, inet_ntoa(*(struct in_addr *)&addr_from));
	ft_printf_fd(1, YELLOW"Dest   addr %s\n"RESET, inet_ntoa(*(struct in_addr *)&dest_addr));

    if ((rcv_sock = open_rcv_socket()) == -1 || (send_sock = open_send_socket()) == -1) {
        goto free_socket;
    }

    uint8_t brut_data[ICMP_DATA_SIZE];
    ft_bzero(brut_data, ICMP_DATA_SIZE);
    gener_random_data(brut_data, ICMP_DATA_SIZE);
    
    t_ping_packet packet = build_ping_packet(addr_from, dest_addr, brut_data);    
    // display_ping_packet(packet);

    errno = 0;
    ssize_t send_ret = sendto(send_sock, &packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr));
    if (send_ret == -1) {
        perror("sendto");
        goto free_socket;
    }
    ft_printf_fd(1, GREEN"Packet sent %u bytes to %s\n"RESET, send_ret, inet_ntoa(*(struct in_addr *)&dest_addr));
    ret = listen_icmp_reply(rcv_sock);

    /* Free socket label */
    free_socket:
    close_multi_socket(rcv_sock, send_sock);
    ft_printf_fd(1, CYAN"Socket closed return in main\n"RESET);
    
    return (ret);
}