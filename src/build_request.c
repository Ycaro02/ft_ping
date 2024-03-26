#include "../include/ft_ping.h"

#include <fcntl.h>
/*
* Get random number % max
*/
uint16_t     gener_uint16(int max)
{
    uint16_t a = 0, b = 0;
	int fd = open("/dev/urandom", O_RDONLY); 
	if (fd != -1)
	{
		char buff[4] = {0};
		int len = read(fd, &buff, 4);
		if (len == -1) {
			printf("Error reading from /dev/urandom\n");
			close(fd);
		}
		// ft_printf_fd(1, YELLOW"RANDOM buff = |%d|%d|%d|%d|\n"RESET, buff[0], buff[1], buff[2], buff[3]);
		a = (*(uint16_t *)buff);
		b = (*(uint16_t *)buff);
		close(fd);
	}
    return ((a + b) % max);
}

/* Get icmp paquets id */
static uint16_t get_icmp_id()
{
	static uint16_t id = 0;

	if (id < UINT16_MAX) {
		++id;
	} else {
		id = 0;
	}
	return (id);
}

/* Get icmp paquets sequence id */
static uint16_t get_icmp_id_seq()
{
	static uint16_t id = 0;

	if (id < UINT16_MAX) {
		++id;
	} else {
		id = 0;
	}
	return (id);
}

t_ping_packet build_ping_packet(char *data)
{
    t_ping_packet packet = {0};

    uint16_t id = get_icmp_id();
    uint16_t seq_id = get_icmp_id_seq();

    packet.iphdr.version = 4;
    packet.iphdr.ihl = 5;
    packet.iphdr.tos = 0;
    packet.iphdr.tot_len = htons(sizeof(t_ping_packet));
    packet.iphdr.id = htons(id);
    packet.iphdr.frag_off = 0;
    packet.iphdr.ttl = 255;
    packet.iphdr.protocol = IPPROTO_ICMP;
    packet.iphdr.check = 0;
    packet.iphdr.saddr = inet_addr("172.31.165.139");
    packet.iphdr.daddr = inet_addr("192.168.1.1");


    packet.icmphdr.type = ICMP_ECHO;
    packet.icmphdr.code = 0;
    packet.icmphdr.checksum = 0;
    packet.icmphdr.un.echo.id = htons(id);
    packet.icmphdr.un.echo.sequence = htons(seq_id);
    packet.icmphdr.checksum = 0;
    ft_memcpy(packet.data, data, ICMP_DATA_SIZE);
    ft_printf_fd(1, "Size of packet struct %u\n", sizeof(t_ping_packet));

    return (packet);
}
