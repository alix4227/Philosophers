/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acrusoe <acrusoe@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 11:08:20 by acrusoe           #+#    #+#             */
/*   Updated: 2025/07/22 11:08:20 by acrusoe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	pose_fourch(t_data *philo)
{
	pthread_mutex_unlock(&philo->fourchs[philo->i]);
	pthread_mutex_unlock(&philo->fourchs[(philo->i + 1) % (philo->nb)]);
}

void	*routine(t_data *philo)
{
	if (philo->i % 2 == 0)
		usleep(philo->tte * 1000);
	while (1)
	{
		take_fourch(philo);
		if (is_simulation_over(philo))
		{
			pose_fourch(philo);
			return (NULL);
		}
		eating_meal(philo);
		if (philo->ac == 6)
		{
			if (count_meals(philo))
				return (NULL);
		}
		pose_fourch(philo);
		if (is_simulation_over(philo))
			return (NULL);
		philo_sleeping(philo);
		philo_thinking(philo);
		usleep(500);
	}
}
