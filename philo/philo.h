/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgarouj <mgarouj@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 09:51:21 by mgarouj           #+#    #+#             */
/*   Updated: 2025/06/25 22:33:27 by mgarouj          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <limits.h>
# include <string.h>
# include <stdint.h> 
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_philo
{
	pthread_t		thread;
	int				id_philo;
	size_t			last_meals_time;
	int				meal_count;
	pthread_mutex_t	l_fork;
	pthread_mutex_t	*r_fork;
	struct s_table	*table;
}					t_philo;

typedef struct s_table
{
	int				num_of_philo;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				limit_meals;
	int				philo_fin_eat;
	int				philo_die;
	int				evry_philo_eat;
	size_t			start_time;
	pthread_mutex_t	table_mutex;
	t_philo			*philo;
}					t_table;

long	ft_atoi(const char *str);
size_t	ft_strlen(const char *s);
int		init_table(t_table *table, char **v);
int		ft_isdigit(int c);
size_t	ft_time_ms(void);
void	ft_usleep(size_t milli, t_table *table);
void	*routine(void *arg);
int		init_philo(t_table *table);
int		thread_creat(t_table *table);
void	ft_destroy_mutex(t_table *table, int flag, int i, int print);
void	ft_destroy_thread(t_table *table);
int		monitor(t_table *table, int *i);
void	unlock_mutex(t_philo *philo);
void	one_philo(t_philo *philo);
int		print_status(char *str, t_philo *philo);

#endif