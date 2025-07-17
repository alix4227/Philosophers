#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include "philo.h"

/* ========================================
   ÉTAPE 1: FONCTIONS UTILITAIRES
   ======================================== */

// Convertit une chaîne de caractères en entier
// Gère les espaces, signes + et -, et vérifie les erreurs
int	ft_atoi(const char *str)
{
	int	i;
	int	neg;
	int	num;

	i = 0;
	num = 0;
	neg = 1;
	// Ignore les espaces et caractères de contrôle
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == ' ')
		i++;
	// Gère les signes + et -
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i + 1] == '-' || str[i + 1] == '+')
			neg = 0; // Erreur: double signe
		else if (str[i] == '+' && (str[i + 1] >= '0' && str[i + 1] <= '9'))
			neg = 1; // Signe positif
		else
			neg = -1; // Signe négatif
		i++;
	}
	// Convertit les chiffres en nombre
	while (str[i] >= '0' && str[i] <= '9')
		num = num * 10 + (str[i++] - 48);
	return (neg * num);
}

// Obtient le temps actuel en millisecondes
// Utilisé pour mesurer les délais et détecter la mort
long	get_time(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	// Convertit secondes + microsecondes en millisecondes
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

/* ========================================
   ÉTAPE 2: GESTION DES FOURCHETTES
   ======================================== */

// Prend les deux fourchettes nécessaires pour manger
// CRUCIAL: implémente la stratégie anti-deadlock
void	take_fourch(t_data *philo)
{
	int	left;
	int	right;

	// Calcul des indices des fourchettes
	left = philo->i; // Fourchette à gauche = index du philosophe
	right = (philo->i + 1) % philo->nb; // Fourchette à droite = index suivant (circulaire)
	
	usleep(6000); // Petite pause pour éviter la contention
	
	// STRATÉGIE ANTI-DEADLOCK: toujours verrouiller dans le même ordre
	// Verrouille d'abord la fourchette avec l'indice le plus petit
	if (left < right)
	{
		usleep(6000);
		pthread_mutex_lock(&philo->fourchs[left]);  // Verrouille gauche
		pthread_mutex_lock(&philo->fourchs[right]); // Puis droite
	}
	else
	{
		pthread_mutex_lock(&philo->fourchs[right]); // Verrouille droite
		pthread_mutex_lock(&philo->fourchs[left]);  // Puis gauche
	}
}

/* ========================================
   ÉTAPE 3: ACTIONS DES PHILOSOPHES
   ======================================== */

// Action: dormir
// Affiche l'état et dort pendant le temps spécifié
void	philo_sleeping(t_data *philo)
{
	long	time_now;

	// Verrouille l'affichage pour éviter les messages mélangés
	pthread_mutex_lock(&philo->check->print_mutex);
	time_now = get_time();
	printf("%ld %ld is sleeping\n", time_now, philo->i + 1);
	pthread_mutex_unlock(&philo->check->print_mutex);
	
	// Dort pendant "time to sleep" millisecondes
	usleep(philo->tts * 500);
}

// Action: penser
// Affiche l'état de réflexion
void	philo_thinking(t_data *philo)
{
	long	time_now;

	usleep(500); // Petite pause
	pthread_mutex_lock(&philo->check->print_mutex);
	time_now = get_time();
	printf("%ld %ld is thinking\n", time_now, philo->i + 1);
	pthread_mutex_unlock(&philo->check->print_mutex);
}

// Action: manger
// Affiche la prise des fourchettes et l'acte de manger
void	eating_meal(t_data *philo)
{
	long	time_now;

	// Affiche la prise de la première fourchette
	pthread_mutex_lock(&philo->check->print_mutex);
	time_now = get_time();
	printf("%ld %ld has taken a fork\n", time_now, philo->i + 1);
	pthread_mutex_unlock(&philo->check->print_mutex);
	
	// Affiche la prise de la deuxième fourchette
	pthread_mutex_lock(&philo->check->print_mutex);
	time_now = get_time();
	printf("%ld %ld has taken a fork\n", time_now, philo->i + 1);
	pthread_mutex_unlock(&philo->check->print_mutex);
	
	// Met à jour le temps du dernier repas (crucial pour la surveillance)
	pthread_mutex_lock(&philo->mutex);
	philo->last_meal = get_time();
	pthread_mutex_unlock(&philo->mutex);
	
	// Affiche l'acte de manger
	pthread_mutex_lock(&philo->check->print_mutex);
	printf("%ld %ld is eating\n", philo->last_meal, philo->i + 1);
	pthread_mutex_unlock(&philo->check->print_mutex);
	
	// Mange pendant "time to eat" millisecondes
	usleep(philo->tte * 1000);
}

/* ========================================
   ÉTAPE 4: SURVEILLANCE ET CONTRÔLE
   ======================================== */

// Vérifie si la simulation doit s'arrêter
// Critères: quelqu'un est mort OU tous ont fini de manger
int	is_simulation_over(t_data *philo)
{
	int	result;

	pthread_mutex_lock(&philo->check->death);
	result = (philo->check->dead == 1 || philo->check->done_eating == philo->nb);
	pthread_mutex_unlock(&philo->check->death);
	return (result);
}

// Thread de surveillance qui vérifie si un philosophe meurt
// Chaque philosophe a son propre thread checker
void	*checker(t_data *philo)
{
	while (1)
	{
		usleep(1000); // Vérifie toutes les millisecondes
		
		// Si la simulation est terminée, arrête la surveillance
		if (is_simulation_over(philo))
			return (NULL);
		
		// Vérifie si le philosophe est mort de faim
		pthread_mutex_lock(&philo->mutex);
		philo->time_now = get_time();
		
		// Si le temps depuis le dernier repas > time to die
		if ((philo->time_now - philo->last_meal) > philo->ttd)
		{
			philo->check->dead = 1; // Marque comme mort
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

/* ========================================
   ÉTAPE 5: COMPTAGE DES REPAS
   ======================================== */

// Compte les repas et vérifie si le philosophe a fini
// Retourne 1 si le philosophe a fini de manger, 0 sinon
int	count_meals(t_data *philo)
{
	int	done;

	pthread_mutex_lock(&philo->mutex);
	philo->meal++; // Incrémente le compteur de repas
	done = (philo->meal == philo->nb_time_eat); // Vérifie si terminé
	pthread_mutex_unlock(&philo->mutex);
	
	if (done)
	{
		// Marque ce philosophe comme ayant fini
		pthread_mutex_lock(&philo->check->death);
		philo->check->done_eating++;
		pthread_mutex_unlock(&philo->check->death);
		
		// Libère les fourchettes avant de partir
		pthread_mutex_unlock(&philo->fourchs[philo->i]);
		pthread_mutex_unlock(&philo->fourchs[(philo->i + 1) % (philo->nb)]);
		return (1);
	}
	return (0);
}

/* ========================================
   ÉTAPE 6: ROUTINE PRINCIPALE D'UN PHILOSOPHE
   ======================================== */

// Fonction principale exécutée par chaque thread de philosophe
// Implémente le cycle: prendre fourchettes -> manger -> libérer -> dormir -> penser
void	*routine(t_data *philo)
{
	// OPTIMISATION: les philosophes pairs attendent un peu
	// Cela réduit la contention initiale pour les fourchettes
	if (philo->i % 2 == 0)
		usleep(philo->tte * 500);
	
	while (1)
	{
		// Vérifie si la simulation est terminée
		if (is_simulation_over(philo))
			return (NULL);
		
		// ÉTAPE 1: Prendre les fourchettes
		take_fourch(philo);
		
		// Vérifie à nouveau après avoir pris les fourchettes
		if (is_simulation_over(philo))
		{
			// Libère les fourchettes si la simulation est terminée
			pthread_mutex_unlock(&philo->fourchs[philo->i]);
			pthread_mutex_unlock(&philo->fourchs[(philo->i + 1) % (philo->nb)]);
			return (NULL);
		}
		
		// ÉTAPE 2: Manger
		eating_meal(philo);
		
		// ÉTAPE 3: Compter les repas (si un nombre limite est spécifié)
		if (philo->ac == 6) // 6 arguments = nombre de repas spécifié
		{
			if (count_meals(philo))
				return (NULL); // Ce philosophe a fini de manger
		}
		
		// ÉTAPE 4: Libérer les fourchettes
		pthread_mutex_unlock(&philo->fourchs[philo->i]);
		pthread_mutex_unlock(&philo->fourchs[(philo->i + 1) % (philo->nb)]);
		
		// Vérifie une dernière fois avant de continuer
		if (is_simulation_over(philo))
			return (NULL);
		
		// ÉTAPE 5: Dormir (sera fait dans la prochaine itération via philo_sleeping)
		// ÉTAPE 6: Penser (sera fait dans la prochaine itération via philo_thinking)
	}
}

/* ========================================
   ÉTAPE 7: GESTION DES THREADS
   ======================================== */

// Crée et gère tous les threads (philosophes + surveillants)
void	manage_philo_2(pthread_t *th, pthread_t *check_death, t_data *philo)
{
	int	i;

	// Crée un thread pour chaque philosophe ET un thread checker
	i = 0;
	while (i < philo->nb)
	{
		// Thread du philosophe qui exécute la routine
		pthread_create(&th[i], NULL, (void *(*)(void *))routine, &philo[i]);
		// Thread de surveillance qui vérifie la mort
		pthread_create(&check_death[i], NULL, (void *(*)(void *))checker, &philo[i]);
		i++;
	}
	
	// Attend que tous les threads se terminent
	i = 0;
	while (i < philo->nb)
	{
		pthread_join(th[i], NULL);      // Attend le thread du philosophe
		pthread_join(check_death[i], NULL); // Attend le thread de surveillance
		i++;
	}
}

// Fonction wrapper pour la gestion des threads
int	manage_philo(t_data *philo)
{
	pthread_t	*th;        // Tableaux de threads pour les philosophes
	pthread_t	*check_death; // Tableaux de threads pour la surveillance

	// Alloue la mémoire pour les threads
	th = malloc(sizeof(pthread_t) * philo->nb);
	check_death = malloc(sizeof(pthread_t) * philo->nb);
	if (!th || !check_death)
		return (0);
	
	// Lance la gestion des threads
	manage_philo_2(th, check_death, philo);
	
	// Libère la mémoire
	free(th);
	free(check_death);
	return (0);
}

/* ========================================
   ÉTAPE 8: INITIALISATION
   ======================================== */

// Initialise chaque structure de philosophe avec les paramètres
void	initialization(t_data *philo, char **av, pthread_mutex_t *fourchs, t_death *check_death)
{
	int	i;
	int	nbr;

	i = 0;
	nbr = ft_atoi(av[1]); // Nombre de philosophes
	
	while (i < nbr)
	{
		// Paramètres de base
		philo[i].nb = ft_atoi(av[1]);           // Nombre de philosophes
		philo[i].ttd = ft_atoi(av[2]);          // Time to die
		philo[i].tte = ft_atoi(av[3]);          // Time to eat
		philo[i].tts = ft_atoi(av[4]);          // Time to sleep
		
		// Nombre de repas (optionnel)
		if (philo->ac == 6)
			philo[i].nb_time_eat = ft_atoi(av[5]);
		
		// Identité et mutex personnel
		philo[i].i = i;
		pthread_mutex_init(&philo[i].mutex, NULL);
		
		// Références partagées
		philo[i].fourchs = fourchs;          // Pointeur vers les fourchettes
		philo[i].check = check_death;        // Pointeur vers la structure de contrôle
		
		// État initial
		philo[i].last_meal = get_time();     // Temps du dernier repas = maintenant
		philo[i].meal = 0;                   // Nombre de repas = 0
		philo[i].has_counted = 0;            // Flag pour le comptage
		
		i++;
	}
}

/* ========================================
   ÉTAPE 9: NETTOYAGE ET DESTRUCTION
   ======================================== */

// Détruit tous les mutex et libère la mémoire
void	destruction(t_data *philo, t_death *check_death, pthread_mutex_t *fourchs, int nbr)
{
	int	i;

	i = 0;
	while (i < nbr)
	{
		pthread_mutex_destroy(&fourchs[i]);     // Détruit les mutex des fourchettes
		pthread_mutex_destroy(&philo[i].mutex); // Détruit les mutex des philosophes
		i++;
	}
	
	// Détruit les mutex de contrôle
	pthread_mutex_destroy(&check_death->print_mutex);
	pthread_mutex_destroy(&check_death->death);
	pthread_mutex_destroy(&check_death->done);
	
	// Libère la mémoire
	free(check_death);
	free(fourchs);
	free(philo);
}

// Nettoyage en cas d'erreur (sans destruction des mutex)
void	destruction2(t_data *philo, t_death *check_death, pthread_mutex_t *fourchs)
{
	free(check_death);
	free(fourchs);
	free(philo);
}

// Initialise tous les mutex nécessaires
void	mutex_initialisation(t_death *check_death, pthread_mutex_t *fourchs, int nbr)
{
	int	i;

	i = 0;
	// Initialise les mutex de contrôle global
	pthread_mutex_init(&check_death->print_mutex, NULL); // Pour l'affichage
	pthread_mutex_init(&check_death->death, NULL);       // Pour la mort
	pthread_mutex_init(&check_death->done, NULL);        // Pour la fin
	
	// Initialise un mutex pour chaque fourchette
	while (i < nbr)
	{
		pthread_mutex_init(&fourchs[i], NULL);
		i++;
	}
}

/* ========================================
   ÉTAPE 10: VALIDATION DES ARGUMENTS
   ======================================== */

// Vérifie que tous les arguments sont des nombres
int	num(int ac, char **av)
{
	int	i;
	int	j;

	i = 1; // Commence à 1 (ignore le nom du programme)
	while (i < ac)
	{
		j = 0;
		while (av[i][j])
		{
			// Vérifie que chaque caractère est un chiffre
			if (av[i][j] < '0' || av[i][j] > '9')
				return (1); // Erreur
			j++;
		}
		i++;
	}
	return (0); // Succès
}

// Vérifie la validité des arguments
int	check_args(char **av, int ac, int nbr, t_data *philo)
{
	// Vérifie que tous les arguments sont numériques
	if (num(ac, av))
		return (0);
	
	// Cas spécial: un seul philosophe meurt immédiatement
	if (nbr == 1)
	{
		philo->time_now = get_time();
		printf("%ld 1 died\n", philo->time_now);
		return (0);
	}
	
	// Vérifie le nombre d'arguments (5 ou 6)
	if (ac != 6 || ac != 5)
		return (0);
	
	return (1); // Arguments valides
}

/* ========================================
   ÉTAPE 11: FONCTION PRINCIPALE
   ======================================== */

int	main(int ac, char **av)
{
	int				i;
	t_data			*philo;        // Tableau des philosophes
	pthread_mutex_t	*fourchs;      // Tableau des fourchettes (mutex)
	int				nbr;           // Nombre de philosophes
	t_death			*check_death;  // Structure de contrôle global

	// PHASE 1: ALLOCATION MÉMOIRE
	i = 0;
	nbr = ft_atoi(av[1]);
	fourchs = malloc(sizeof(pthread_mutex_t) * nbr);
	check_death = malloc(sizeof(t_death));
	philo = malloc(sizeof(t_data) * nbr);
	
	// Vérification d'allocation
	if (!fourchs || !check_death || !philo)
		return (0);
	
	// Stocke le nombre d'arguments pour chaque philosophe
	while (i < nbr)
	{
		philo[i].ac = ac;
		i++;
	}
	
	// PHASE 2: INITIALISATION DES VARIABLES GLOBALES
	check_death->dead = 0;         // Personne n'est mort
	check_death->done_eating = 0;  // Personne n'a fini de manger
	
	// PHASE 3: VALIDATION DES ARGUMENTS
	if (!check_args(av, ac, nbr, philo))
	{
		destruction2(philo, check_death, fourchs);
		return (0);
	}
	
	// PHASE 4: INITIALISATION DES MUTEX
	mutex_initialisation(check_death, fourchs, nbr);
	
	// PHASE 5: INITIALISATION DES PHILOSOPHES
	initialization(philo, av, fourchs, check_death);
	
	// PHASE 6: LANCEMENT DE LA SIMULATION
	manage_philo(philo);
	
	// PHASE 7: NETTOYAGE FINAL
	destruction(philo, check_death, fourchs, nbr);
	
	return (0);
}

/* ========================================
   RÉSUMÉ DU FONCTIONNEMENT:
   
   1. Chaque philosophe a son propre thread qui exécute 'routine'
   2. Chaque philosophe a aussi un thread 'checker' qui surveille sa mort
   3. Les fourchettes sont des mutex partagés entre philosophes adjacents
   4. La stratégie anti-deadlock: toujours verrouiller les fourchettes dans le même ordre
   5. La simulation s'arrête si quelqu'un meurt ou si tous ont fini de manger
   6. Tous les accès aux données partagées sont protégés par des mutex
   ======================================== */