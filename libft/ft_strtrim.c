/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nfour <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/01 14:57:50 by nfour             #+#    #+#             */
/*   Updated: 2022/10/01 14:59:15 by nfour            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_is_in_charset(char c, char const *set)
{
	int	i;

	i = 0;
	while (set[i])
	{
		if (set[i] == c)
			return (1);
		i++;
	}
	return (0);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*str;
	size_t	size;
	int		i;

	if (s1 == NULL || set == NULL)
		return (NULL);
	i = 0;
	size = ft_strlen(s1);
	while (ft_is_in_charset(s1[i], set) == 1)
		i++;
	if (s1[i] == '\0')
		return (ft_strdup(""));
	while (ft_is_in_charset(s1[size - 1], set) == 1)
		size--;
	str = ft_calloc(sizeof(char), (size - i) + 1);
	if (str == NULL)
		return (NULL);
	while (size != (size_t)i && (int)(size - i - 1) >= 0)
	{
		str[size - i - 1] = s1[size - 1];
		size--;
	}
	return (str);
}
