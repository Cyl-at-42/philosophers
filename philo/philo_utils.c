
#include "philo.h"

t_timestamp	philo_report(t_philo *philo, t_timestamp timestamp, char *reportmsg)
{
	pthread_mutex_lock(&philo->simul->report_mutex);
	if (!reportmsg)
		philo->simul->simul_state = STOPPED;
	if (philo->simul->simul_state == STOPPED)
		return (pthread_mutex_unlock(&philo->simul->report_mutex), 0);
	printf("%ld %d %s\n", (timestamp - philo->simul->beginning_ts) / 1000,
		philo->id, reportmsg);
	return (pthread_mutex_unlock(&philo->simul->report_mutex), timestamp);
}

t_timestamp	philo_change_state(t_philo *philo, int new_state)
{
	static char	*report_msgs[5] = {NULL, "is thinking", "is eating",
		"is sleeping", "died"};
	t_timestamp	timestamp;

	timestamp = get_current_timestamp();
	pthread_mutex_lock(&philo->state_mutex);
	philo->state = new_state;
	if (new_state == PHILO_STATE_EATING)
	{
		philo->be_nice = (timestamp - philo->last_meal_timestamp)
			< (philo->simul->time_to_eat + philo->simul->time_to_sleep + 5000);
		philo->last_meal_timestamp = timestamp;
	}
	pthread_mutex_unlock(&philo->state_mutex);
	if (new_state <= PHILO_STATE_DEAD)
		timestamp = philo_report(philo, timestamp, report_msgs[new_state]);
	if (new_state == PHILO_STATE_DEAD)
		timestamp = philo_report(philo, 0, NULL);
	return (timestamp);
}

void	philo_putback_forks(t_philo *philo)
{
	t_mutex	*right_fork_mutex;

	if (philo->id == philo->simul->number_of_philosophers)
		right_fork_mutex = &philo->simul->philo_table[0].left_fork_mutex;
	else
		right_fork_mutex
			= &philo->simul->philo_table[philo->id].left_fork_mutex;
	if (philo->id & 1)
	{
		pthread_mutex_unlock(right_fork_mutex);
		pthread_mutex_unlock(&philo->left_fork_mutex);
	}
	else
	{
		pthread_mutex_unlock(&philo->left_fork_mutex);
		pthread_mutex_unlock(right_fork_mutex);
	}
}
