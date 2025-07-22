/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleaning.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acrusoe <acrusoe@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 10:59:12 by acrusoe           #+#    #+#             */
/*   Updated: 2025/07/22 10:59:12 by acrusoe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	destruction(t_data *philo, t_death *check_death,
	pthread_mutex_t *fourchs, int nbr)
{
	int	i;

	i = 0;
	while (i < nbr)
	{
		pthread_mutex_destroy(&fourchs[i]);
		pthread_mutex_destroy(&philo[i].mutex);
		i++;
	}
	pthread_mutex_destroy(&check_death->print_mutex);
	pthread_mutex_destroy(&check_death->death);
	pthread_mutex_destroy(&check_death->done);
	free(check_death);
	free(fourchs);
	free(philo);
}

void	destruction2(t_data *philo, t_death *check_death,
	pthread_mutex_t *fourchs)
{
	free(check_death);
	free(fourchs);
	free(philo);
}
