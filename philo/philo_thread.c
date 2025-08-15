
#include "philo.h"

static void	_philo_get_forks_order(t_philo *philo, t_mutex **fork1_mutex_ptr,
	t_mutex **fork2_mutex_ptr)
{
	if (philo->id == philo->simul->number_of_philosophers)
		*fork1_mutex_ptr = &philo->simul->philo_table[0].left_fork_mutex;
	else
		*fork1_mutex_ptr
			= &philo->simul->philo_table[philo->id].left_fork_mutex;
	if (philo->id & 1)
	{
		*fork2_mutex_ptr = &philo->left_fork_mutex;
	}
	else
	{
		*fork2_mutex_ptr = *fork1_mutex_ptr;
		*fork1_mutex_ptr = &philo->left_fork_mutex;
	}
}

static void	_double_unlock(t_mutex *mutex1, t_mutex *mutex2)
{
	pthread_mutex_unlock(mutex1),
	pthread_mutex_unlock(mutex2);
}

// Philo with odd id are right-handling
// Philo with even id are left-handling
// Returns false if philo is dead during waiting for forks
// or if simul has stopped
bool	philo_take_forks(t_philo *philo)
{
	t_mutex		*fork1_mutex;
	t_mutex		*fork2_mutex;

	_philo_get_forks_order(philo, &fork1_mutex, &fork2_mutex);
	pthread_mutex_lock(fork1_mutex);
	pthread_mutex_lock(&philo->state_mutex);
	if (philo->state == PHILO_STATE_DEAD)
		return (_double_unlock(fork1_mutex, &philo->state_mutex), false);
	if (!philo_report(philo, get_current_timestamp(), "has taken a fork"))
		return (_double_unlock(fork1_mutex, &philo->state_mutex), false);
	if (philo->simul->number_of_philosophers == 1)
		return (usleep(philo->simul->time_to_die),
			_double_unlock(fork1_mutex, &philo->state_mutex), false);
	pthread_mutex_unlock(&philo->state_mutex);
	pthread_mutex_lock(fork2_mutex);
	pthread_mutex_lock(&philo->state_mutex);
	if (philo->state == PHILO_STATE_DEAD)
		return (_double_unlock(fork1_mutex, fork2_mutex),
			pthread_mutex_unlock(&philo->state_mutex), false);
	if (!philo_report(philo, get_current_timestamp(), "has taken a fork"))
		return (_double_unlock(fork1_mutex, fork2_mutex),
			pthread_mutex_unlock(&philo->state_mutex), false);
	return (pthread_mutex_unlock(&philo->state_mutex), true);
}

// Return value: Finish philo simulation if true
static bool	_philo_cycle(t_philo *philo)
{
	t_timestamp	timestamp;
	t_timestamp	t_of_death;

	if (!philo_take_forks(philo))
		return (true);
	timestamp = philo_change_state(philo, PHILO_STATE_EATING);
	if (!timestamp)
		return (philo_putback_forks(philo), true);
	usleep(philo->simul->time_to_eat);
	philo_putback_forks(philo);
	philo->meal_count++;
	if (philo->simul->meal_count_max == philo->meal_count)
		return (philo_change_state(philo, PHILO_STATE_FULL), true);
	timestamp = philo_change_state(philo, PHILO_STATE_SLEEPING);
	if (!timestamp)
		return (true);
	pthread_mutex_lock(&philo->state_mutex);
	t_of_death = philo->last_meal_timestamp + philo->simul->time_to_die;
	pthread_mutex_unlock(&philo->state_mutex);
	if (t_of_death >= (timestamp + philo->simul->time_to_sleep))
		return (usleep(philo->simul->time_to_sleep), false);
	if (t_of_death > timestamp)
		usleep(t_of_death - timestamp);
	return (philo_change_state(philo, PHILO_STATE_DEAD), true);
}

int	philo(t_philo *philo)
{
	t_timestamp	timestamp;

	pthread_mutex_lock(&philo->simul->report_mutex);
	if (philo->simul->simul_state != SIMUL_RUNNING)
		return (pthread_mutex_unlock(&philo->simul->report_mutex), 0);
	if (!philo->simul->beginning_ts)
		philo->simul->beginning_ts = get_current_timestamp();
	if (!philo->last_meal_timestamp)
		philo->last_meal_timestamp = philo->simul->beginning_ts;
	pthread_mutex_unlock(&philo->simul->report_mutex);
	while (1)
	{
		timestamp = philo_change_state(philo, PHILO_STATE_THINKING);
		if (!timestamp)
			return (0);
		if (philo->be_nice)
			usleep(philo->be_nice * 1000);
		if (_philo_cycle(philo))
			return (0);
	}
	return (0);
}
