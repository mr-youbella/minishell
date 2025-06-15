/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 21:35:08 by youbella          #+#    #+#             */
/*   Updated: 2024/11/17 07:38:05 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *haystack, const char *needle, size_t n)
{
	size_t	i;
	size_t	len_needle;

	i = 0;
	len_needle = ft_strlen(needle);
	if (!len_needle)
		return ((char *)haystack);
	while (haystack[i] && i + len_needle <= n)
	{
		if (ft_strncmp(&haystack[i], needle, len_needle) == 0)
			return (&((char *)haystack)[i]);
		i++;
	}
	return (NULL);
}
