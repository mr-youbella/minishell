/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 20:24:24 by youbella          #+#    #+#             */
/*   Updated: 2024/11/17 01:13:22 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t n)
{
	size_t	i;
	size_t	len_dst;
	size_t	len_src;
	size_t	min;

	i = 0;
	len_src = ft_strlen(src);
	min = n;
	if (n == 0)
		return (len_src + min);
	len_dst = ft_strlen(dst);
	if (min > len_dst)
		min = len_dst;
	if (n <= len_dst)
		return (len_src + min);
	while (src[i] && i + len_dst < n - 1)
	{
		dst[len_dst + i] = src[i];
		i++;
	}
	dst[len_dst + i] = 0;
	return (len_src + min);
}
