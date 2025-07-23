/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acrusoe <acrusoe@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 10:55:08 by acrusoe           #+#    #+#             */
/*   Updated: 2025/07/22 10:55:08 by acrusoe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	num(int ac, char **av)
{
	int	i;
	int	j;

	i = 1;
	while (i < ac)
	{
		j = 0;
		while (av[i][j])
		{
			if (av[i][j] < '0' || av[i][j] > '9')
				return (1);
			j++;
		}
		i++;
	}
	return (0);
}

int	check_args(char **av, int ac, int nbr, t_data *philo)
{
	if (num(ac, av))
		return (0);
	if (ac != 6 && ac != 5)
		return (0);
	if (nbr == 1)
	{
		philo->time_now = get_time() - philo->start_time;
		printf("%ld 1 died\n", philo->time_now);
		return (0);
	}
	return (1);
}
