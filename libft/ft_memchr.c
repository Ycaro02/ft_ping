/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nfour <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/01 14:43:28 by nfour             #+#    #+#             */
/*   Updated: 2022/10/01 14:43:45 by nfour            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	unsigned char	*str;
	unsigned int	i;

	i = 0;
	str = (unsigned char *)s;
	while (n != 0)
	{
		if (str[i] == (unsigned char)c)
			return ((void *)&str[i]);
		i++;
		n--;
	}
	return (NULL);
}
