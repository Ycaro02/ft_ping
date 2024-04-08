#include "../include/ft_ping.h"

/**
 * @brief Display time in milliseconds
 * @param color color to display
 * @param time time in microseconds
 * @param last if it's the last time to display
*/
void display_ms_time(char *color, suseconds_t time, uint8_t last)
{
	uint32_t mod = time % 1000;

	dprintf(1, "%s%li.%03i", color, (time / 1000), mod);

	if (!last) {
		ft_printf_fd(1, "/");
	} else {
		ft_printf_fd(1, " ms\n");
	}
}

/**
 * @brief Get current time in microseconds
 * @return current time in microseconds
*/
suseconds_t  get_ms_time(void)
{
	t_timeval  now;

	if (gettimeofday(&now, NULL) != 0)
			return (0);
	return ((now.tv_sec * 1000000) + now.tv_usec);
}