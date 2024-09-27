#include "../include/ft_ping.h"

/**
 *	@brief Get random number % max
 *	@param max max value
*/
uint32_t     gener_uint32(int max)
{
    uint32_t res = 0;
	int fd = open("/dev/urandom", O_RDONLY); 
	if (fd != -1) {
		char buff[4] = {0};
		int len = read(fd, &buff, 4);
		if (len != -1) {
            res = (*(uint32_t *)buff);
            close(fd);
		}
	}
    return (res % max);
}

/**
 *	@brief Generate random data for icmp packet
 *	@param buff: buffer to store random data
 *	@param size: size of the buffer
*/
void gener_random_data(uint8_t *buff, int size)
{
    for (int i = 0; i < size; i++) {
            buff[i] = gener_uint32(255);
    }
}