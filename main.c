/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acrusoe <acrusoe@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 10:49:20 by acrusoe           #+#    #+#             */
/*   Updated: 2025/07/22 10:49:20 by acrusoe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	manage_philo_2(pthread_t *th, pthread_t *check_death, t_data *philo)
{
	int	i;

	i = 0;
	while (i < philo->nb)
	{
		pthread_create(&th[i], NULL, (void *(*)(void *))routine, &philo[i]);
		pthread_create(&check_death[i], NULL,
			(void *(*)(void *))checker, &philo[i]);
		i++;
	}
	i = 0;
	while (i < philo->nb)
	{
		pthread_join(th[i], NULL);
		pthread_join(check_death[i], NULL);
		i++;
	}
}

int	manage_philo(t_data *philo)
{
	pthread_t	*th;
	pthread_t	*check_death;

	th = malloc(sizeof(pthread_t) * philo->nb);
	check_death = malloc(sizeof(pthread_t) * philo->nb);
	if (!th || !check_death)
		return (0);
	manage_philo_2(th, check_death, philo);
	free(th);
	free(check_death);
	return (0);
}

int	main(int ac, char **av)
{
	t_data			*philo;
	pthread_mutex_t	*fourchs;
	int				nbr;
	t_death			*check_death;

	nbr = ft_atoi(av[1]);
	variables_init(nbr, &fourchs, &philo, &check_death);
	check_death->dead = 0;
	check_death->done_eating = 0;
	if (!check_args(av, ac, nbr, philo))
	{
		destruction2(philo, check_death, fourchs);
		return (0);
	}
	get_number_arguments(philo, nbr, ac);
	mutex_initialisation(check_death, fourchs, nbr);
	initialization(philo, av, fourchs, check_death);
	manage_philo(philo);
	destruction(philo, check_death, fourchs, nbr);
	return (0);
}
