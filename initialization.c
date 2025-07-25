/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialization.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acrusoe <acrusoe@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 10:57:00 by acrusoe           #+#    #+#             */
/*   Updated: 2025/07/22 10:57:00 by acrusoe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	initialization(t_data *philo, char **av, pthread_mutex_t *fourchs,
	t_death *check_death)
{
	int		i;
	int		nbr;
	long	start;

	i = 0;
	start = get_time();
	nbr = ft_atoi(av[1]);
	while (i < nbr)
	{
		philo[i].nb = ft_atoi(av[1]);
		philo[i].ttd = ft_atoi(av[2]);
		philo[i].tte = ft_atoi(av[3]);
		philo[i].tts = ft_atoi(av[4]);
		if (philo->ac == 6)
			philo[i].nb_time_eat = ft_atoi(av[5]);
		philo[i].i = i;
		philo[i].start_time = start;
		pthread_mutex_init(&philo[i].mutex, NULL);
		philo[i].fourchs = fourchs;
		philo[i].last_meal = 0;
		philo[i].check = check_death;
		philo[i].meal = 0;
		philo[i].has_counted = 0;
		i++;
	}
}

void	mutex_initialisation(t_death *check_death,
	pthread_mutex_t *fourchs, int nbr)
{
	int	i;

	i = 0;
	pthread_mutex_init(&check_death->print_mutex, NULL);
	pthread_mutex_init(&check_death->death, NULL);
	pthread_mutex_init(&check_death->done, NULL);
	while (i < nbr)
	{
		pthread_mutex_init(&fourchs[i], NULL);
		i++;
	}
}

void	variables_init(int nbr, pthread_mutex_t **fourchs, t_data **philo,
	t_death **check_death)
{
	*fourchs = malloc(sizeof(pthread_mutex_t) * nbr);
	*check_death = malloc(sizeof(t_death));
	*philo = malloc(sizeof(t_data) * nbr);
	if (!*fourchs || !*check_death || !*philo)
		return ;
}

void	get_number_arguments(t_data *philo, int nbr, int ac)
{
	int	i;

	i = 0;
	while (i < nbr)
	{
		philo[i].ac = ac;
		i++;
	}
}
