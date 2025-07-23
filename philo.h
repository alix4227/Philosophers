/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acrusoe <acrusoe@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 10:49:33 by acrusoe           #+#    #+#             */
/*   Updated: 2025/07/22 10:49:33 by acrusoe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>

typedef struct s_death
{
	int				dead;
	int				done_eating;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	death;
	pthread_mutex_t	done;
}	t_death;

typedef struct s_data
{
	long			i;
	long			nb;
	long			last_meal;
	long			meal;
	long			end;
	long			ttd;
	long			tts;
	long			tte;
	long			nb_time_eat;
	long			time_now;
	long			has_counted;
	long			start_time;
	int				ac;
	t_death			*check;
	pthread_mutex_t	*fourchs;
	pthread_mutex_t	mutex;
}	t_data;

void	destruction(t_data *philo, t_death *check_death,
			pthread_mutex_t *fourchs, int nbr);
void	destruction2(t_data *philo, t_death *check_death,
			pthread_mutex_t *fourchs);
int		is_simulation_over(t_data *philo);
void	*checker(t_data *philo);
void	initialization(t_data *philo, char **av, pthread_mutex_t *fourchs,
			t_death *check_death);
void	mutex_initialisation(t_death *check_death,
			pthread_mutex_t *fourchs, int nbr);
void	variables_init(int nbr, pthread_mutex_t **fourchs, t_data **philo,
			t_death **check_death);
void	get_number_arguments(t_data *philo, int nbr, int ac);	
void	manage_philo_2(pthread_t *th, pthread_t *check_death, t_data *philo);
int		manage_philo(t_data *philo);
int		num(int ac, char **av);
int		check_args(char **av, int ac, int nbr, t_data *philo);
void	take_fourch(t_data *philo);
void	philo_sleeping(t_data *philo);
void	philo_thinking(t_data *philo);
void	eating_meal(t_data *philo);
int		count_meals(t_data *philo);
void	*routine(t_data *philo);
int		ft_atoi(const char *str);
long	get_time(void);
void	pose_fourch(t_data *philo);
#endif