#include "../include/ft_ping.h"

int g_signal_received = 0;

/* Ip address string format to bin format */
in_addr_t ipv4_strtoaddr(char *str)
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
    in_addr_t local_host = ipv4_strtoaddr(LOCAL_HOST_ADDR);

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

t_context init_ping_context(char *dest_addr)
{
    t_context context;

    ft_bzero(&context, sizeof(context));
    context.src_addr = get_process_ipv4_addr();
    context.dst_sockaddr.sin_family = AF_INET;
    context.dst_sockaddr.sin_addr.s_addr = ipv4_strtoaddr(dest_addr);
    context.send_sock = open_send_socket();
    context.rcv_sock = open_rcv_socket();
    if (context.send_sock == -1 || context.rcv_sock == -1) {
        close_multi_socket(context.send_sock, context.rcv_sock);
        exit(1);
    }
    return (context);    
}

int main(int argc, char **argv)
{
    t_context       c;
    in_addr_t       dest_addr;
    int8_t          ret = 1;
    

    if (argc < 2) {
        ft_printf_fd(2, PURPLE"%s: usage error: Destination address required\n"RESET, argv[0]);
        return (1);
    }
    /* Program can exit here */
    c = init_ping_context(argv[1]);
    dest_addr = c.dst_sockaddr.sin_addr.s_addr;

    t_ping_packet packet = build_ping_packet(c.src_addr, dest_addr);    
    errno = 0;
    ssize_t send_ret = sendto(c.send_sock, &packet, sizeof(packet), 0, (struct sockaddr *)&c.dst_sockaddr, sizeof(c.dst_sockaddr));
    if (send_ret == -1) {
        perror("sendto");
        goto free_socket;
    }
    ft_printf_fd(1, GREEN"Packet sent %u bytes to %s\n"RESET, send_ret, inet_ntoa(*(struct in_addr *)&dest_addr));

    ret = listen_icmp_reply(c.rcv_sock);

    /* Free socket label */
    free_socket:
    close_multi_socket(c.rcv_sock, c.send_sock);
    ft_printf_fd(1, CYAN"Socket closed return in main\n"RESET);
    
    return (ret);
}