
#include "philo_bonus.h"

bool	_philo_thread_sleep(t_philo *philo)
{
	t_timestamp	timestamp;
	t_timestamp	t_of_death;

	timestamp = philo_change_state(philo, PHILO_STATE_SLEEPING);
	if (!timestamp)
		return (false);
	t_of_death = philo->last_meal_timestamp + philo->simul->time_to_die;
	if (t_of_death >= (timestamp + philo->simul->time_to_sleep))
		usleep(philo->simul->time_to_sleep);
	else
	{
		if (t_of_death > timestamp)
			usleep(t_of_death - timestamp);
		return (philo_change_state(philo, PHILO_STATE_DEAD), false);
	}
	return (true);
}

static bool	_philo_thread_take_forks2(t_philo *philo)
{
	if (!philo_change_state(philo, PHILO_STATE_THINKING))
		return (false);
	if (philo->simul->number_of_philosophers == 1)
	{
		philo_report(philo, get_current_timestamp(), "has taken a fork");
		return (usleep (philo->simul->time_to_die), false);
	}
	return (true);
}

bool	_philo_thread_take_forks(t_philo *philo)
{
	if (!_philo_thread_take_forks2(philo))
		return (false);
	if (philo->be_nice)
		usleep(philo->be_nice * 1000);
	sem_wait(philo->simul->fork_sem);
	sem_wait(philo->simul->state_sem);
	if ((philo->state == PHILO_STATE_DEAD)
		|| (philo->simul->death_sem->__align == 1))
		return (sem_post(philo->simul->state_sem),
			sem_post(philo->simul->fork_sem), false);
	sem_post(philo->simul->state_sem);
	philo_report(philo, get_current_timestamp(), "has taken a fork");
	if (philo->simul->number_of_philosophers == 1)
		return (sem_post(philo->simul->fork_sem), false);
	sem_wait(philo->simul->fork_sem);
	sem_wait(philo->simul->state_sem);
	if ((philo->state == PHILO_STATE_DEAD)
		|| (philo->simul->death_sem->__align == 1))
		return (sem_post(philo->simul->state_sem),
			sem_post(philo->simul->fork_sem),
			sem_post(philo->simul->fork_sem), false);
	return (sem_post(philo->simul->state_sem),
		philo_report(philo, get_current_timestamp(), "has taken a fork"), true);
}

void	*philo_thread(void *ptr)
{
	t_philo		*philo;

	philo = (t_philo *)ptr;
	while (true)
	{
		if (!_philo_thread_take_forks(philo))
			return (NULL);
		if (!philo_change_state(philo, PHILO_STATE_EATING))
			return (NULL);
		usleep(philo->simul->time_to_eat);
		sem_post(philo->simul->fork_sem);
		sem_post(philo->simul->fork_sem);
		philo->meal_count++;
		if (philo->meal_count == philo->simul->meal_count_max)
			return (philo_change_state(philo, PHILO_STATE_FULL), NULL);
		if (!_philo_thread_sleep(philo))
			return (NULL);
	}
	return (NULL);
}
