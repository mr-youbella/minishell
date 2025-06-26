/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils_1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgarouj <mgarouj@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 09:46:31 by mgarouj           #+#    #+#             */
/*   Updated: 2025/06/25 19:08:50 by mgarouj          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philo.h"

size_t	ft_strlen(const char *s)
{
	size_t	len;

	len = 0;
	while (s[len])
		len++;
	return (len);
}

int	ft_isdigit(int c)
{
	if ((c >= '0' && c <= '9'))
		return (1);
	return (0);
}

static int	sign_spc(char *str, int *skip)
{
	int	i;
	int	sign;

	i = 0;
	sign = 1;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == 32)
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	*skip = i;
	return (sign);
}

long	ft_atoi(const char *str)
{
	int			i;
	int			sign;
	long long	result;
	long long	o;

	i = 0;
	result = 0;
	sign = sign_spc((char *)str, &i);
	while (((char *)str)[i] >= '0' && ((char *)str)[i] <= '9')
	{
		o = result;
		result = result * 10 + (((char *)str)[i++] - 48);
		if (result < o)
			return (-1);
	}
	return (result * sign);
}

void	ft_usleep(size_t milli, t_table *table)
{
	size_t	now;
	size_t	elapsed;

	now = ft_time_ms();
	elapsed = ft_time_ms();
	while (elapsed - now < milli)
	{
		pthread_mutex_lock(&table->table_mutex);
		if (table->philo_die)
		{
			pthread_mutex_unlock(&table->table_mutex);
			break ;
		}
		pthread_mutex_unlock(&table->table_mutex);
		usleep(333);
		elapsed = ft_time_ms();
	}
}
