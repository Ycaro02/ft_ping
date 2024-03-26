#include "../include/ft_ping.h"

uint16_t compute_checksum(uint16_t *data, size_t size)
{
	uint32_t    sum = 0;
	uint16_t    *ptr = data;

	/* Sum all 16-bit words */
	while (size > 1) {
		sum += ntohs(*ptr);
		size -= 2;
		ptr++;
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
