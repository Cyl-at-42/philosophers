
#include "philo_bonus.h"

t_timestamp	philo_report(t_philo *philo, t_timestamp timestamp, char *reportmsg)
{
	sem_wait(philo->simul->report_sem);
	printf("%ld %d %s\n", (timestamp - philo->simul->beginning_ts) / 1000,
		philo->id, reportmsg);
	sem_post(philo->simul->report_sem);
	return (timestamp);
}

t_timestamp	philo_change_state(t_philo *philo, int new_state)
{
	static char	*report_msgs[5] = {NULL, "is thinking", "is eating",
		"is sleeping", "died"};
	t_timestamp	timestamp;

	timestamp = get_current_timestamp();
	sem_wait(philo->simul->state_sem);
	if (philo->simul->death_sem->__align)
		return (sem_post(philo->simul->state_sem), 0);
	philo->state = new_state;
	if (new_state == PHILO_STATE_EATING)
	{
		philo->be_nice = (timestamp - philo->last_meal_timestamp)
			< (philo->simul->time_to_eat + philo->simul->time_to_sleep + 5000);
		philo->last_meal_timestamp = timestamp;
	}
	if (new_state <= PHILO_STATE_DEAD)
		timestamp = philo_report(philo, timestamp, report_msgs[new_state]);
	if (new_state == PHILO_STATE_DEAD)
	{
		philo->simul->death_sem->__align = 1;
		timestamp = 0;
	}
	sem_post(philo->simul->state_sem);
	return (timestamp);
}

static bool	_philo_check_deadly_thinking(t_philo *philo)
{
	t_timestamp	timestamp;

	if (philo->state != PHILO_STATE_THINKING)
		return (false);
	timestamp = get_current_timestamp();
	if ((timestamp - philo->last_meal_timestamp) > philo->simul->time_to_die)
	{
		philo->state = PHILO_STATE_DEAD;
		if (philo->simul->death_sem->__align == 0)
			philo_report(philo, timestamp, "died");
		philo->simul->death_sem->__align = 1;
		return (true);
	}
	return (false);
}

static bool	_philo_loop(t_philo *philo)
{
	sem_wait(philo->simul->state_sem);
	if (_philo_check_deadly_thinking(philo))
	{
		sem_post(philo->simul->state_sem);
		return (true);
	}
	if (philo->state == PHILO_STATE_FULL || philo->simul->death_sem->__align)
	{
		sem_post(philo->simul->state_sem);
		return (true);
	}
	sem_post(philo->simul->state_sem);
	usleep(5000);
	return (false);
}

int	philo(t_philo *philo)
{
	t_tid	philo_thread_tid;

	sem_wait(philo->simul->report_sem);
	sem_post(philo->simul->report_sem);
	philo->last_meal_timestamp = philo->simul->beginning_ts;
	sem_wait(philo->simul->state_sem);
	if (philo->simul->death_sem->__align)
		return (sem_post(philo->simul->state_sem), EXIT_FAILURE);
	sem_post(philo->simul->state_sem);
	if (pthread_create(&philo_thread_tid, NULL, philo_thread,
			(void *)philo) == -1)
		return (write_error("Thread create error\n"),
			sem_wait(philo->simul->state_sem),
			philo->simul->death_sem->__align = 1,
			sem_post(philo->simul->state_sem), EXIT_FAILURE);
	while (1)
		if (_philo_loop(philo))
			break ;
	return (pthread_join(philo_thread_tid, NULL), EXIT_SUCCESS);
}
