
#include "philo.h"

bool	init_simulation_params(t_simulation *simulation, int argc, char **argv)
{
	int	param_int;

	simulation->beginning_ts = 0;
	simulation->meal_count_max = 0;
	if (!is_arg_valid(argv[1], &simulation->number_of_philosophers))
		return (write_error("number_of_philosophers\n"), false);
	if (!is_arg_valid(argv[2], &param_int))
		return (write_error("time_to_die\n"), false);
	simulation->time_to_die = param_int * 1000;
	if (!is_arg_valid(argv[3], &param_int))
		return (write_error("time_to_eat\n"), false);
	simulation->time_to_eat = param_int * 1000;
	if (!is_arg_valid(argv[4], &param_int))
		return (write_error("time_to_sleep\n"), false);
	simulation->time_to_sleep = param_int * 1000;
	if (argc != 6)
		return (true);
	if (!is_arg_valid(argv[5], &simulation->meal_count_max))
		return (write_error("number_of_times_each_philosopher_must_eat\n"),
			false);
	return (true);
}

bool	init_simulation(t_simulation *simul)
{
	size_t	table_size;
	int		i;

	table_size = simul->number_of_philosophers * sizeof(t_philo);
	simul->philo_table = (t_philo *)malloc(table_size);
	if (!simul->philo_table)
		return (write_error("Can't allocate memory\n"), 1);
	memset(simul->philo_table, 0, table_size);
	simul->simul_state = INIT_MUTEX;
	if (pthread_mutex_init(&simul->report_mutex, NULL))
		return (write_error("Can't init mutex\n"), false);
	simul->simul_state = INIT_MUTEXES;
	i = 0;
	while (i < simul->number_of_philosophers)
	{
		if (pthread_mutex_init(&simul->philo_table[i].left_fork_mutex, NULL))
			return (write_error("Can't init mutexes\n"), false);
		simul->philo_table[i].init_fork_mutex_ok = true;
		if (pthread_mutex_init(&simul->philo_table[i].state_mutex, NULL))
			return (write_error("Can't init mutexes\n"), false);
		simul->philo_table[i].init_state_mutex_ok = true;
		i++;
	}
	return (true);
}

bool	init_simulation_threads(t_simulation *simul)
{
	int	i;

	simul->simul_state = INIT_THREADS;
	pthread_mutex_lock(&simul->report_mutex);
	i = 0;
	while (i < simul->number_of_philosophers)
	{
		simul->philo_table[i].simul = simul;
		simul->philo_table[i].id = i + 1;
		if (pthread_create(&simul->philo_table[i].thread_id, NULL,
				(void *)philo, (void *) &simul->philo_table[i]))
			return (write_error("Thread creation failed\n"),
				pthread_mutex_unlock(&simul->report_mutex), false);
		i++;
	}
	simul->simul_state = SIMUL_RUNNING;
	pthread_mutex_unlock(&simul->report_mutex);
	return (true);
}
