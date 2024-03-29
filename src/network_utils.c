#include "../include/ft_ping.h"

/* Ip address string format to bin format */
in_addr_t ipv4_str_toaddr(char *str)
{
    struct in_addr addr;

    /* Convert presentation format to binary network format */
    if (inet_pton(AF_INET, str, &addr) <= 0) {
        ft_printf_fd(2, RED"ft_ping: %s: Name or service not known\n"RESET, str);
        return (0);
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
    in_addr_t addr = 0, tmp = 0, local_host = ipv4_str_toaddr(LOCAL_HOST_ADDR);

    errno = 0;
    if (getifaddrs(&ifa_head) == -1) {
        perror("getifaddrs");
        return (0);
    }
    for (current = ifa_head; current != NULL; current = current->ifa_next) {
        if (current->ifa_addr && current->ifa_addr->sa_family == AF_INET) {
            // ft_printf_fd(1, PURPLE"Interface addr %s\n"RESET, inet_ntoa(((struct sockaddr_in *)current->ifa_addr)->sin_addr));
            // ft_printf_fd(1, PURPLE"Interface name %s\n"RESET, current->ifa_name);
            tmp = ((struct sockaddr_in *)current->ifa_addr)->sin_addr.s_addr;
            if (tmp != local_host) {
                addr = tmp;
                ft_printf_fd(1, PURPLE"Process addr %s\n"RESET, inet_ntoa(*(struct in_addr *)&addr));
                break ;
            }
        }
    }
    freeifaddrs(ifa_head);
    return (addr);
}



static void display_hostent(struct hostent *host)
{
    int i;

    ft_printf_fd(1, "h_name: %s\n", host->h_name);
    ft_printf_fd(1, "h_aliases: ");
    for (i = 0; host->h_aliases[i] != NULL; i++) {
        ft_printf_fd(1, "%s ", host->h_aliases[i]);
    }
    ft_printf_fd(1, "\n");
    ft_printf_fd(1, "h_addrtype: %d\n", host->h_addrtype);
    ft_printf_fd(1, "h_length: %d\n", host->h_length);
    ft_printf_fd(1, "h_addr_list: ");
    for (i = 0; host->h_addr_list[i] != NULL; i++) {
        ft_printf_fd(1, "%s ", inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));
    }
    ft_printf_fd(1, "\n");
}

in_addr_t hostname_to_ipv4_addr(char *hostname)
{
    struct hostent *host = NULL;
    in_addr_t addr = 0;

    sethostent(1); 
    host = gethostbyname(hostname);
    if (host == NULL) {
        ft_printf_fd(2, PURPLE"ft_ping: %s: Name or service not known\n"RESET, hostname);
        endhostent();
        return (0);
    }
    display_hostent(host);
    addr = *(in_addr_t *)host->h_addr_list[0];
    endhostent();
    return (addr);
}