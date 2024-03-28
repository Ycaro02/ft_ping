#include "../include/ft_ping.h"

suseconds_t  get_ms_time(void)
{
        t_timeval  now;

        if (gettimeofday(&now, NULL) != 0)
                return (0);
        return ((now.tv_sec * 1000000) + now.tv_usec);
}