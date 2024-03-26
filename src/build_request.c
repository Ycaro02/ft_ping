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
uint16_t get_icmp_id_seq()
{
	static uint16_t id = 0;

	if (id < UINT16_MAX) {
		++id;
	} else {
		id = 0;
	}
	return (id);
}

t_ping_packet build_ping_packet(in_addr_t addr_from, in_addr_t addr_dest, char *data)
{
    t_ping_packet packet = {0};

    uint16_t id = get_icmp_id();
    // uint16_t seq_id = get_icmp_id_seq();

	/* Build IP packet header */
	/* Ip version */
    packet.iphdr.version = 4;
    /* Number for dword (uint16) */
	packet.iphdr.ihl = 5;
    packet.iphdr.tos = 0;
    /* packet len */
	packet.iphdr.tot_len = htons(sizeof(t_ping_packet));
    /* packet id */
	packet.iphdr.id = htons((getpid() & 0xFFFF));
    packet.iphdr.frag_off = 0;
    /* packet time to live */
	packet.iphdr.ttl = 64;
	/* packet payload protocole */
    packet.iphdr.protocol = IPPROTO_ICMP;
	/* packet checksum */
    packet.iphdr.check = 0;
	/* Ip source address */
    packet.iphdr.saddr = addr_from;
    /* Ip destination address */
	(void)addr_dest;
	/* Define the data as a char array */
	packet.iphdr.daddr = addr_dest;


	/* Build ICMP packet header */
	/* Icmp paquet type */
    packet.icmphdr.type = ICMP_ECHO;
	/* Icmp paquet code */
    packet.icmphdr.code = 0;
	/* Icmp paquet checksum */
    packet.icmphdr.checksum = 0;
	/* Icmp paquet id */
    packet.icmphdr.un.echo.id = htons(id);
	/* Icmp paquet sequence id */
    packet.icmphdr.un.echo.sequence = htons(0);
	/* Icmp payload data */
    ft_memcpy(packet.data, data, ICMP_DATA_SIZE);
    ft_printf_fd(1, "Size of packet struct %u\n", sizeof(t_ping_packet));

	/* Compute ICMP checksum */
	packet.icmphdr.checksum = compute_checksum((uint16_t *)&packet.icmphdr, ICMP_HDR_SIZE + ICMP_DATA_SIZE);
	/* Compute IP checksum */
	packet.iphdr.check = compute_checksum((uint16_t *)&packet.iphdr, IP_HDR_SIZE);

    return (packet);
}
