/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   death_checker.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acrusoe <acrusoe@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 11:02:27 by acrusoe           #+#    #+#             */
/*   Updated: 2025/07/22 11:02:27 by acrusoe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	is_simulation_over(t_data *philo)
{
	int	result;

	pthread_mutex_lock(&philo->check->death);
	result = (philo->check->dead == 1
			|| philo->check->done_eating == philo->nb);
	pthread_mutex_unlock(&philo->check->death);
	return (result);
}

void	*checker(t_data *philo)
{
	while (1)
	{
		usleep(500);
		if (is_simulation_over(philo))
			return (NULL);
		pthread_mutex_lock(&philo->mutex);
		philo->time_now = get_time() - philo->start_time;
		if ((philo->time_now - philo->last_meal) > philo->ttd)
		{
			philo->check->dead = 1;
			pthread_mutex_lock(&philo->check->death);
			pthread_mutex_lock(&philo->check->print_mutex);
			printf("%ld %ld died\n", philo->time_now, philo->i + 1);
			pthread_mutex_unlock(&philo->check->print_mutex);
			pthread_mutex_unlock(&philo->check->death);
			pthread_mutex_unlock(&philo->mutex);
			return (NULL);
		}
		pthread_mutex_unlock(&philo->mutex);
	}
}
