#include "../include/ft_ping.h"

/* Get icmp paquets id */
static uint16_t get_icmp_id()
{
	static uint16_t id = 0;
	
	if (id == 0) {
		id = gener_uint32((UINT16_MAX >> 1));
		id += gener_uint32((UINT16_MAX >> 1));
	}

	return (id);
}

static void fill_ip_header(t_context *c, t_iphdr *iphdr)
{
	/* Build IP packet header */
	/* version and ihl are uint8 val no need to apply host to newwork function */
	iphdr->version = 4;
	iphdr->ihl = 5;
	iphdr->tos = 0;
	iphdr->tot_len = htons(sizeof(t_ping_packet));
	iphdr->id = htons((getpid() & 0xFFFF));
	iphdr->frag_off = 0;

	if (has_flag(c->flag, T_OPTION)) {
		iphdr->ttl = c->opt_value.ttl;
	} else {
		iphdr->ttl = 64;
	}

	iphdr->protocol = IPPROTO_ICMP;
	iphdr->check = 0;
}

t_ping_packet build_ping_packet(t_context *c, in_addr_t addr_from, in_addr_t addr_dest)
{
    t_ping_packet packet;
    uint16_t id = get_icmp_id();

	ft_bzero(&packet, sizeof(packet));

	/* Build IP packet header */
	fill_ip_header(c, &packet.iphdr);
    packet.iphdr.saddr = addr_from;
	packet.iphdr.daddr = addr_dest;

	/* Build ICMP packet header */
    packet.icmphdr.type = ICMP_ECHO;
    packet.icmphdr.code = 0;
    packet.icmphdr.checksum = 0;
    packet.icmphdr.un.echo.id = htons(id);
    packet.icmphdr.un.echo.sequence = 0;

	/* Build ICMP timestamp */
	gettimeofday((struct timeval *)packet.data , NULL);
	/* Build ICMP data */
    gener_random_data(packet.data + ICMP_TIMESTAMP_SIZE, (ICMP_DATA_SIZE - ICMP_TIMESTAMP_SIZE));

	/* Compute ICMP checksum */
	packet.icmphdr.checksum = compute_checksum((uint16_t *)&packet.icmphdr, ICMP_HDR_SIZE + ICMP_DATA_SIZE);
	/* Compute IP checksum */
	packet.iphdr.check = compute_checksum((uint16_t *)&packet.iphdr, IP_HDR_SIZE);

    return (packet);
}
