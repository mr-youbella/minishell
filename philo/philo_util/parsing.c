/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgarouj <mgarouj@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 09:38:13 by mgarouj           #+#    #+#             */
/*   Updated: 2025/06/25 19:01:45 by mgarouj          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philo.h"

size_t	ft_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000ULL + tv.tv_usec / 1000ULL);
}

int	check_int_max(t_table *table)
{
	if (table->num_of_philo <= 0 || table->num_of_philo > INT_MAX)
		return (0);
	if (table->time_to_die <= 0 || table->time_to_die > INT_MAX)
		return (0);
	if (table->time_to_eat <= 0 || table->time_to_eat > INT_MAX)
		return (0);
	if (table->time_to_sleep <= 0 || table->time_to_sleep > INT_MAX)
		return (0);
	if (table->limit_meals != -1 && (table->limit_meals <= 0
			|| table->limit_meals > INT_MAX))
		return (0);
	return (1);
}

int	check_args(char *str, t_table *table)
{
	int	i;

	i = 0;
	(void )table;
	if (str[i] == '+')
		i++;
	while (str[i] == '0')
		i++;
	while (str[i])
		if (!ft_isdigit(str[i++]))
			return (0);
	return (1);
}

void	init_ndes(t_table *table, char **v)
{
	table->num_of_philo = ft_atoi(v[1]);
	table->time_to_die = ft_atoi (v[2]);
	table->time_to_eat = ft_atoi(v[3]);
	table->time_to_sleep = ft_atoi(v[4]);
	table->philo_fin_eat = 0;
}

int	init_table(t_table *table, char **v)
{
	int	i;

	i = 1;
	while (v[i])
		if (!check_args(v[i++], table))
			return (write(2, "argument not valide \n", 22), 0);
	init_ndes(table, v);
	if (table->num_of_philo > 200)
		return (write(2, "argument not valide \n", 22), 0);
	if (v[5])
		table->limit_meals = ft_atoi(v[5]);
	else
		table->limit_meals = -1;
	table->philo = malloc(sizeof(t_philo) * table->num_of_philo);
	if (!table->philo)
		return (write(2, "failed allocat philo\n", 22), 0);
	if (pthread_mutex_init(&table->table_mutex, NULL) != 0)
		return (free(table->philo), write(2, "error pthread mutex\n", 21), 0);
	table->evry_philo_eat = 0;
	table->philo_die = 0;
	table->start_time = ft_time_ms();
	if (!check_int_max(table))
		return (ft_destroy_mutex(table, 't', 1, 1), 0);
	return (1);
}
