/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acrusoe <acrusoe@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 14:26:05 by acrusoe           #+#    #+#             */
/*   Updated: 2025/05/09 14:26:05 by acrusoe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include "philo.h"

int	ft_atoi(const char *str)
{
	int	i;
	int	neg;
	int	num;

	i = 0;
	num = 0;
	neg = 1;
	if (!str)
		return (0);
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == ' ')
		i++;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i + 1] == '-' || str[i + 1] == '+')
			neg = 0;
		else if (str[i] == '+' && (str[i + 1] >= '0' && str[i + 1] <= '9'))
			neg = 1;
		else
			neg = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
		num = num * 10 + (str[i++] - 48);
	return (neg * num);
}

long	get_time(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

void	take_fourch(t_data *philo)
{
	int	left;
	int	right ;

	left = philo->i;
	right = (philo->i + 1) % philo->nb;
	usleep(6000);
	if (left < right)
	{
		usleep(6000);
		pthread_mutex_lock(&philo->fourchs[left]);
		pthread_mutex_lock(&philo->fourchs[right]);
	}
	else
	{
		pthread_mutex_lock(&philo->fourchs[right]);
		pthread_mutex_lock(&philo->fourchs[left]);
	}
}

void	philo_sleeping(t_data *philo)
{
	long	time_now;

	pthread_mutex_lock(&philo->check->print_mutex);
	time_now = get_time();
	printf("%ld %ld is sleeping\n", time_now, philo->i + 1);
	pthread_mutex_unlock(&philo->check->print_mutex);
	usleep(philo->tts * 500);
}

void	philo_thinking(t_data *philo)
{
	long	time_now;

	usleep(500);
	pthread_mutex_lock(&philo->check->print_mutex);
	time_now = get_time();
	printf("%ld %ld is thinking\n", time_now, philo->i + 1);
	pthread_mutex_unlock(&philo->check->print_mutex);
}

void	eating_meal(t_data *philo)
{
	long			time_now;

	pthread_mutex_lock(&philo->check->print_mutex);
	time_now = get_time();
	printf("%ld %ld has taken a fork\n", time_now, philo->i + 1);
	pthread_mutex_unlock(&philo->check->print_mutex);
	pthread_mutex_lock(&philo->check->print_mutex);
	time_now = get_time();
	printf("%ld %ld has taken a fork\n", time_now, philo->i + 1);
	pthread_mutex_unlock(&philo->check->print_mutex);
	pthread_mutex_lock(&philo->mutex);
	philo->last_meal = get_time();
	pthread_mutex_unlock(&philo->mutex);
	pthread_mutex_lock(&philo->check->print_mutex);
	printf("%ld %ld is eating\n", philo->last_meal, philo->i + 1);
	pthread_mutex_unlock(&philo->check->print_mutex);
	pthread_mutex_unlock(&philo->fourchs[philo->i]);
	pthread_mutex_unlock(&philo->fourchs[(philo->i + 1) % (philo->nb)]);
	usleep(philo->tte * 1000);
}

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
		usleep(1000);
		if (is_simulation_over(philo))
			return (NULL);
		pthread_mutex_lock(&philo->mutex);
		philo->time_now = get_time();
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
		return (1);
	}
	return (0);
}

void	*routine(t_data *philo)
{
	if (philo->i % 2 == 0)
		usleep(philo->tte * 500);
	while (1)
	{
		if (is_simulation_over(philo))
			return (NULL);
		take_fourch(philo);
		if (is_simulation_over(philo))
		{
			pthread_mutex_unlock(&philo->fourchs[philo->i]);
			pthread_mutex_unlock(&philo->fourchs[(philo->i + 1) % (philo->nb)]);
			return (NULL);
		}
		eating_meal(philo);
		if (philo->ac == 6)
		{
			if (count_meals(philo))
				return (NULL);
		}
		if (is_simulation_over(philo))
			return (NULL);
		philo_sleeping(philo);
		philo_thinking(philo);
	}
}

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

void	initialization(t_data *philo, char **av, pthread_mutex_t *fourchs,
	t_death *check_death)
{
	int	i;
	int	nbr;

	i = 0;
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
		pthread_mutex_init(&philo[i].mutex, NULL);
		philo[i].fourchs = fourchs;
		philo[i].last_meal = get_time();
		philo[i].check = check_death;
		philo[i].meal = 0;
		philo[i].has_counted = 0;
		i++;
	}
}

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
		philo->time_now = get_time();
		printf("%ld 1 died\n", philo->time_now);
		return (0);
	}
	return (1);
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
