#include "../include/ft_ping.h"

/* Ip address string format to bin format */
in_addr_t ipv4_str_toaddr(char *str)
{
    struct in_addr addr;

    /* Convert presentation format to binary network format */
    if (inet_pton(AF_INET, str, &addr) <= 0) {
        ft_printf_fd(2, RED"ft_ping: %s: Name or service not known\n"RESET, str);
        exit(1);
    }
    return (addr.s_addr);
}

/**
 *	@brief Get process ipv4 address
 *	@return in_addr_t ipv4 address of the process
*/
in_addr_t get_process_ipv4_addr()
{
    struct ifaddrs *ifa_head, *current;
    in_addr_t addr = 0, local_host = ipv4_str_toaddr(LOCAL_HOST_ADDR);

    errno = 0;
    if (getifaddrs(&ifa_head) == -1) {
        perror("getifaddrs");
        return (0);
    }
    for (current = ifa_head; current != NULL; current = current->ifa_next) {
        if (current->ifa_addr && current->ifa_addr->sa_family == AF_INET) {
            addr = ((struct sockaddr_in *)current->ifa_addr)->sin_addr.s_addr;
            if (addr != local_host) {
                ft_printf_fd(1, PURPLE"Process addr %s\n"RESET, inet_ntoa(*(struct in_addr *)&addr));
                break ;
            }
        }
    }
    freeifaddrs(ifa_head);
    return (addr);
}

