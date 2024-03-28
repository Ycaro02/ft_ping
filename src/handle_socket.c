#include "../include/ft_ping.h"

/* @brief Open socker*/
int open_send_socket(void)
{
    int sockfd = -1, disable = 1;

    errno = 0;
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
        perror("socket");
        return (-1);
    }

    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &disable, sizeof(disable)) < 0) {
        perror("setsockopt disable auto ip header failed");
        return (-1);
    }
    return (sockfd);
}


int open_rcv_socket(void)
{
    int 	sockfd;
	struct	timeval tv;
	
	tv.tv_sec = 10;
	tv.tv_usec = 0;

    errno = 0;

    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        perror("socket");
        return (-1);
    }
    
	errno = 0;
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0) {
		perror("setsockopt set timeout failed");
        return (-1);
	}
    return (sockfd);
}

/* @brief Close socket */
int close_socket(int sock)
{
    errno = 0;
    if (close(sock) == -1) {
        perror("close");
        return (-1);
    }
    return (1);
}