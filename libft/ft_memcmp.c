/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nfour <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/01 14:44:11 by nfour             #+#    #+#             */
/*   Updated: 2022/10/01 14:45:20 by nfour            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	int				i;
	unsigned char	*s1_char;
	unsigned char	*s2_char;

	s1_char = (unsigned char *)s1;
	s2_char = (unsigned char *)s2;
	i = 0;
	while (n != 0)
	{
		if (s1_char[i] != s2_char[i])
			return (s1_char[i] - s2_char[i]);
		i++;
		n--;
	}
	return (0);
}
