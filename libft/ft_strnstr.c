/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nfour <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/29 13:09:26 by nfour             #+#    #+#             */
/*   Updated: 2022/10/01 15:33:59 by nfour            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t		i;
	size_t		little_len;

	little_len = ft_strlen(little);
	i = 0;
	if (big == NULL && len == 0)
		return (NULL);
	if (little_len == 0)
		return ((char *)big);
	while (big[i] != '\0' && i < len)
	{
		if (ft_strncmp(&big[i], &little[0], little_len) == 0
			&& i + little_len - 1 < len)
			return ((char *)&big[i]);
		i++;
	}
	return (NULL);
}
