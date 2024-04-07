#include "../include/ft_ping.h"

void display_ms_time(char *color, suseconds_t time, uint8_t last)
{
	uint32_t mod = time % 1000;

	// if (mod < 100) {
	// 	ft_printf_fd(1, "%s%i.0%i", color, (time / 1000), mod);
	// } else if (mod < 10) {
	// 	ft_printf_fd(1, "%s%i.00%i", color, (time / 1000), mod);
	// } else {
	// 	ft_printf_fd(1, "%s%i.%i", color, (time / 1000), mod);
	// }

	dprintf(1, "%s%li.%03i", color, (time / 1000), mod);

	if (!last) {
		ft_printf_fd(1, "/");
	} else {
		ft_printf_fd(1, " ms\n");
	}
}

suseconds_t  get_ms_time(void)
{
        t_timeval  now;

        if (gettimeofday(&now, NULL) != 0)
                return (0);
        return ((now.tv_sec * 1000000) + now.tv_usec);
}