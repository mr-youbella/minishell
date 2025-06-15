/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 02:43:17 by youbella          #+#    #+#             */
/*   Updated: 2024/11/17 07:52:30 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static short	check(const char *set, char c)
{
	while (*set)
	{
		if (*set == c)
			return (1);
		set++;
	}
	return (0);
}

char	*ft_strtrim(const char *s, const char *set)
{
	size_t	start;
	size_t	end;

	if (!s || !set)
		return (NULL);
	start = 0;
	if (!*s)
		return (ft_strdup(""));
	end = ft_strlen(s) - 1;
	while (s[start] && check(set, s[start]))
		start++;
	while (end >= 0 && check(set, s[end]))
		end--;
	return (ft_substr(s, start, (end - start) + 1));
}
