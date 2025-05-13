/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acrusoe <acrusoe@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 14:56:04 by acrusoe           #+#    #+#             */
/*   Updated: 2025/05/09 14:56:04 by acrusoe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>e

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
	int				ac;
	t_death			*check;
	pthread_mutex_t	*fourchs;
	pthread_mutex_t	mutex;
}	t_data;

#endif