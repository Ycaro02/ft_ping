#include "../include/ft_ping.h"

/* Get icmp paquets id */
static uint16_t get_icmp_id()
{
	static uint16_t id = 1;

	if (id == UINT16_MAX) {
		id = 1;
	}
	return (id++);
}

t_ping_packet build_ping_packet(in_addr_t addr_from, in_addr_t addr_dest)
{
    t_ping_packet packet;
    uint16_t id = get_icmp_id();

	ft_bzero(&packet, sizeof(packet));
	/* Build IP packet header */
	/* version and ihl are uint8 val no need to apply host to newwork function */
    packet.iphdr.version = 4;
	packet.iphdr.ihl = 5;
    packet.iphdr.tos = 0;
	packet.iphdr.tot_len = htons(sizeof(t_ping_packet));
	packet.iphdr.id = htons((getpid() & 0xFFFF));
    packet.iphdr.frag_off = 0;
	packet.iphdr.ttl = 64;
    packet.iphdr.protocol = IPPROTO_ICMP;
    packet.iphdr.check = 0;
    packet.iphdr.saddr = addr_from;
	packet.iphdr.daddr = addr_dest;

	/* Build ICMP packet header */
    packet.icmphdr.type = ICMP_ECHO;
    packet.icmphdr.code = 0;
    packet.icmphdr.checksum = 0;
    packet.icmphdr.un.echo.id = htons(id);
    packet.icmphdr.un.echo.sequence = 0;
    gener_random_data(packet.data, ICMP_DATA_SIZE);

	/* Compute ICMP checksum */
	packet.icmphdr.checksum = compute_checksum((uint16_t *)&packet.icmphdr, ICMP_HDR_SIZE + ICMP_DATA_SIZE);
	/* Compute IP checksum */
	packet.iphdr.check = compute_checksum((uint16_t *)&packet.iphdr, IP_HDR_SIZE);

    return (packet);
}
