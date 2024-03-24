# include "libft.h"

/**
 * Get abs value
*/
uint32_t abs_diff(uint32_t a, uint32_t b)
{
    int64_t diff = (int64_t)a - (int64_t)b;

    /* if diff < 0 return (diff * -1) else return diff*/
    return (diff < 0 ? (diff * -1) : diff);
}

/**
 * Get max value, return first if ==
*/
uint32_t max_uint32(uint32_t a, uint32_t b)
{
    return (a >= b ? a : b);
}

/**
 * Get min value, return first if ==
*/
uint32_t min_uint32(uint32_t a, uint32_t b)
{
    return (a <= b ? a : b);
}
