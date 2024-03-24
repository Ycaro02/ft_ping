/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nfour <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/01 14:45:41 by nfour             #+#    #+#             */
/*   Updated: 2022/10/01 14:45:52 by nfour            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char	*dest_i;
	unsigned char	*src_i;

	dest_i = (unsigned char *)dest;
	src_i = (unsigned char *)src;
	if (src == NULL && dest == NULL)
		return (dest);
	while (n != 0)
	{
		dest_i[n - 1] = src_i[n - 1];
		n--;
	}
	return (dest);
}
