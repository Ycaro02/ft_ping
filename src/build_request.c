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

static uint8_t char_to_hexa(char c)
{
	if (ft_isdigit(c)) {
		return (c - '0');
	} else if (ft_isalpha(c)) {
		if (c >= 'A' && c <= 'F') {
			return (c - 'A' + 10);
		} else {
			return (c - 'a' + 10);
		}
	}
	return (0);
}

static void fill_hexa_buff(uint8_t *buff, char *pattern, uint32_t size)
{
	uint32_t i = 0;
	uint32_t j = 0;


	while (j < size) {
		buff[j] = (char_to_hexa(pattern[i]) << 4);
		++i;
		if (pattern[i] == '\0') {
			i = 0;
		}
		buff[j] |= char_to_hexa(pattern[i]);
		++i;
		++j;
		if (pattern[i] == '\0') {
			i = 0;
		}
	}
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
	if (has_flag(c->flag, P_OPTION)) {
		/* Gener random byte for first 6 bytes */		
    	gener_random_data(packet.data + ICMP_TIMESTAMP_SIZE, (ICMP_BRUT_DATA_BYTES - 5)); /* (ICMP_BRUT_DATA_BYTES - 5) let 5 00 before pattern data*/
		fill_hexa_buff(packet.data + ICMP_TIMESTAMP_SIZE + ICMP_BRUT_DATA_BYTES, c->opt_value.pattern, (ICMP_DATA_SIZE - ICMP_TIMESTAMP_SIZE - ICMP_BRUT_DATA_BYTES));
	} else {
    	gener_random_data(packet.data + ICMP_TIMESTAMP_SIZE, (ICMP_DATA_SIZE - ICMP_TIMESTAMP_SIZE));
	}

	/* Compute ICMP checksum */
	packet.icmphdr.checksum = compute_checksum((uint16_t *)&packet.icmphdr, ICMP_HDR_SIZE + ICMP_DATA_SIZE);
	/* Compute IP checksum */
	packet.iphdr.check = compute_checksum((uint16_t *)&packet.iphdr, IP_HDR_SIZE);

    return (packet);
}
