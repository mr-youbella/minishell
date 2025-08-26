/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 17:44:20 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 05:10:58 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static short	overflow(size_t result, size_t b_result, short sign)
{
	if ((result / 10 != b_result && sign == 1)
		|| (result >= LONG_MAX && sign == 1))
		return (-1);
	if ((result / 10 != b_result && sign == -1)
		|| (result > LONG_MAX && sign == -1))
		return (0);
	return (1);
}

int	ft_atoi(const char *str)
{
	size_t	i;
	size_t	result;
	size_t	b_result;
	short	sign;

	1 && (i = 0, result = 0, sign = 1);
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == 32)
		i++;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign *= -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		b_result = result;
		result = result * 10 + str[i++] - 48;
		if (overflow(result, b_result, sign) != 1)
			return (-1);
	}
	if (str[i])
		return (-3);
	return (result * sign);
}
