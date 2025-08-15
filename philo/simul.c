
#include "philo.h"

void	is_simulation_finished2(t_simulation *sim, int i)
{
	t_timestamp	timestamp;

	pthread_mutex_lock(&sim->philo_table[i].state_mutex);
	if (sim->philo_table[i].state == PHILO_STATE_THINKING)
	{
		timestamp = get_current_timestamp();
		if (timestamp
			>= (sim->philo_table[i].last_meal_timestamp + sim->time_to_die))
			sim->death_detected = true;
	}
	if (sim->meal_count_max)
		if (sim->philo_table[i].state == PHILO_STATE_FULL)
			sim->completely_full_count++;
	if (sim->philo_table[i].state == PHILO_STATE_DEAD)
		sim->one_is_dead = true;
	pthread_mutex_unlock(&sim->philo_table[i].state_mutex);
}

bool	is_simulation_finished(t_simulation *sim)
{
	int	i;

	sim->completely_full_count = 0;
	sim->one_is_dead = false;
	sim->death_detected = false;
	i = 0;
	while (i < sim->number_of_philosophers)
	{
		is_simulation_finished2(sim, i);
		if (sim->death_detected)
			philo_change_state(&sim->philo_table[i], PHILO_STATE_DEAD);
		if (sim->one_is_dead || sim->death_detected)
			return (true);
		i++;
	}
	return (sim->completely_full_count == sim->number_of_philosophers);
}

bool	is_arg_valid(char *arg_str, int *arg_val_ptr)
{
	int		int_val;
	bool	is_valid;

	arg_str = parse_int(&int_val, &is_valid, arg_str);
	if (*arg_str || !is_valid)
		return (write_error("Invalid argument: "), false);
	if (int_val <= 0)
		return (write_error("Invalid argument: "), false);
	*arg_val_ptr = int_val;
	return (true);
}

void	simul_clean(t_simulation *simul)
{
	int	i;

	if (simul->simul_state >= INIT_MUTEX)
		pthread_mutex_destroy(&simul->report_mutex);
	if (simul->simul_state >= INIT_MUTEXES)
	{
		i = 0;
		while (i < simul->number_of_philosophers)
		{
			if (simul->philo_table[i].init_fork_mutex_ok)
				pthread_mutex_destroy(&simul->philo_table[i].left_fork_mutex);
			if (simul->philo_table[i].init_state_mutex_ok)
				pthread_mutex_destroy(&simul->philo_table[i].state_mutex);
			i++;
		}
	}
	free(simul->philo_table);
}

void	simul_join_threads(t_simulation *simul)
{
	int	i;

	i = 0;
	while (i < simul->number_of_philosophers)
	{
		if (simul->philo_table[i].thread_id)
			pthread_join(simul->philo_table[i].thread_id, NULL);
		i++;
	}
}
