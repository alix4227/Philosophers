/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_functions2.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acrusoe <acrusoe@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 17:20:36 by acrusoe           #+#    #+#             */
/*   Updated: 2025/07/23 17:23:01 by acrusoe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	take_left_fourch_first(t_data *philo, int left,
	int right, long time_now)
{
	pthread_mutex_lock(&philo->fourchs[left]);
	if (is_simulation_over(philo))
	{
		pthread_mutex_unlock(&philo->fourchs[left]);
		return ;
	}
	pthread_mutex_lock(&philo->check->print_mutex);
	time_now = get_time() - philo->start_time;
	printf("%ld %ld has taken a fork\n", time_now, philo->i + 1);
	pthread_mutex_unlock(&philo->check->print_mutex);
	pthread_mutex_lock(&philo->fourchs[right]);
	pthread_mutex_lock(&philo->check->print_mutex);
	time_now = get_time() - philo->start_time;
	printf("%ld %ld has taken a fork\n", time_now, philo->i + 1);
	pthread_mutex_unlock(&philo->check->print_mutex);
}

void	take_right_fourch_first(t_data *philo, int right, long time_now)
{
	pthread_mutex_lock(&philo->fourchs[right]);
	if (is_simulation_over(philo))
	{
		pthread_mutex_unlock(&philo->fourchs[right]);
		return ;
	}
	pthread_mutex_lock(&philo->check->print_mutex);
	time_now = get_time() - philo->start_time;
	printf("%ld %ld has taken a fork\n", time_now, philo->i + 1);
	pthread_mutex_unlock(&philo->check->print_mutex);
	pthread_mutex_lock(&philo->fourchs[philo->i]);
	pthread_mutex_lock(&philo->check->print_mutex);
	time_now = get_time() - philo->start_time;
	printf("%ld %ld has taken a fork\n", time_now, philo->i + 1);
	pthread_mutex_unlock(&philo->check->print_mutex);
}

void	take_fourch(t_data *philo)
{
	int		left;
	int		right;
	long	time_now;

	left = philo->i;
	right = (philo->i + 1) % philo->nb;
	time_now = 0;
	if (left < right)
		take_left_fourch_first(philo, left, right, time_now);
	else
		take_right_fourch_first(philo, right, time_now);
}
