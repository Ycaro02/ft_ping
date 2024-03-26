#include "../include/ft_ping.h"

uint16_t compute_checksum(uint16_t *data, size_t size)
{
	uint32_t    sum = 0;
	uint16_t    *ptr = data;

	/* Sum all 16-bit words */
	while (size > 1) {
		sum += ntohs(*ptr++);
		size -= 2;
	}
	/* No need to convert simple bytes */
	if (size > 0) {
		sum += *(uint8_t *)ptr;
	}
	/*  Fold 32-bit sum to 16 bits */
	while (sum >> SHORT_INT_BITS) {
		sum = ((sum & 0xffff) + (sum >> SHORT_INT_BITS));
	}
	/* One complement return */
	return (htons(~sum));
}

uint8_t verify_checksum(char *buffer, uint16_t ip_checksum, uint16_t icmp_checksum)
{
	t_ping_packet	packet;
	uint16_t		checksum;

	/* Read size of ip header struct in buffer and store it in associate struct */
	packet.iphdr = *((t_iphdr *)buffer);
	/* Same here but we add size of iphdr to buff to skip ip header*/
	packet.icmphdr = *((t_icmphdr *)(buffer + IP_HDR_SIZE));
	ft_memcpy(packet.data, buffer + IP_HDR_SIZE + ICMP_HDR_SIZE, ICMP_DATA_SIZE);
	/* Set ip and icmp checksum field to 0 to compute */
	packet.iphdr.check = 0;
	packet.icmphdr.checksum = 0;
	// ft_printf_fd(1, YELLOW"\nDisplay packet after reset checksum\n"RESET);
	// display_ping_packet(packet);
	checksum = compute_checksum((uint16_t *)&packet.icmphdr, ICMP_HDR_SIZE + ICMP_DATA_SIZE);
	ft_printf_fd(1, YELLOW"\nCompute ICMP checksum: %u real: %u\n"RESET, ntohs(checksum), ntohs(icmp_checksum));
	if (icmp_checksum != checksum) {
		ft_printf_fd(1, RED"ICMP checksum is wrong\n"RESET);
		return (0);
	}
	packet.icmphdr.checksum = icmp_checksum;

	checksum = compute_checksum((uint16_t *)&packet.iphdr, PACKET_SIZE);
	ft_printf_fd(1, YELLOW"Compute IP checksum: %u real: %u\n"RESET, ntohs(checksum), ntohs(ip_checksum));
	if (ip_checksum != checksum) {
		ft_printf_fd(1, RED"IP checksum is wrong\n"RESET);
		return (0);
	}
	return (1);
}
