/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nfour <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/01 14:00:28 by nfour             #+#    #+#             */
/*   Updated: 2022/10/01 14:51:07 by nfour            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char			*dst;
	size_t			len;
	unsigned int	i;

	i = 0;
	if (s == NULL)
		return (NULL);
	len = ft_strlen(s);
	dst = ft_calloc(sizeof(char), len + 1);
	if (dst == NULL)
		return (NULL);
	while (s[i])
	{
		dst[i] = f(i, s[i]);
		i++;
	}
	return (dst);
}
