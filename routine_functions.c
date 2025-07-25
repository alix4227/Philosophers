/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_functions.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acrusoe <acrusoe@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 11:07:03 by acrusoe           #+#    #+#             */
/*   Updated: 2025/07/22 11:07:03 by acrusoe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	philo_sleeping(t_data *philo)
{
	long	time_now;
	if (is_simulation_over(philo))
		return ;
	pthread_mutex_lock(&philo->check->print_mutex);
	time_now = get_time() - philo->start_time;
	printf("%ld %ld is sleeping\n", time_now, philo->i + 1);
	pthread_mutex_unlock(&philo->check->print_mutex);
	usleep(philo->tts * 1000);
}

void	philo_thinking(t_data *philo)
{
	long	time_now;

	usleep(500);
	if (is_simulation_over(philo))
		return ;
	pthread_mutex_lock(&philo->check->print_mutex);
	time_now = get_time() - philo->start_time;
	printf("%ld %ld is thinking\n", time_now, philo->i + 1);
	pthread_mutex_unlock(&philo->check->print_mutex);
}

void	eating_meal(t_data *philo)
{
	if (is_simulation_over(philo))
		return ;
	pthread_mutex_lock(&philo->mutex);
	philo->last_meal = get_time() - philo->start_time;
	pthread_mutex_unlock(&philo->mutex);
	pthread_mutex_lock(&philo->check->print_mutex);
	printf("%ld %ld is eating\n", philo->last_meal, philo->i + 1);
	pthread_mutex_unlock(&philo->check->print_mutex);
	usleep(philo->tte * 1000);
}

int	count_meals(t_data *philo)
{
	int	done;

	pthread_mutex_lock(&philo->mutex);
	philo->meal++;
	done = (philo->meal == philo->nb_time_eat);
	pthread_mutex_unlock(&philo->mutex);
	if (done)
	{
		pthread_mutex_lock(&philo->check->death);
		philo->check->done_eating++;
		pthread_mutex_unlock(&philo->check->death);
		pthread_mutex_unlock(&philo->fourchs[philo->i]);
		pthread_mutex_unlock(&philo->fourchs[(philo->i + 1) % (philo->nb)]);
		return (1);
	}
	return (0);
}
