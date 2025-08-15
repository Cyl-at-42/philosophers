
#include "philo_bonus.h"

static bool	_is_arg_valid(char *arg_str, int *arg_val_ptr)
{
	int		int_val;
	bool	is_valid;

	arg_str = parse_int(&int_val, &is_valid, arg_str);
	if (*arg_str || !is_valid)
		return (write_error("Invalid argument: "), false);
	if (int_val < 0)
		return (write_error("Invalid argument: "), false);
	*arg_val_ptr = int_val;
	return (true);
}

bool	init_simulation_params(t_simulation *simulation, int argc, char **argv)
{
	int	param_int;

	simulation->beginning_ts = 0;
	simulation->meal_count_max = 0;
	if (!_is_arg_valid(argv[1], &simulation->number_of_philosophers))
		return (write_error("number_of_philosophers\n"), false);
	if (!_is_arg_valid(argv[2], &param_int))
		return (write_error("time_to_die\n"), false);
	simulation->time_to_die = param_int * 1000;
	if (!_is_arg_valid(argv[3], &param_int))
		return (write_error("time_to_eat\n"), false);
	simulation->time_to_eat = param_int * 1000;
	if (!_is_arg_valid(argv[4], &param_int))
		return (write_error("time_to_sleep\n"), false);
	simulation->time_to_sleep = param_int * 1000;
	if (argc != 6)
		return (true);
	if (!_is_arg_valid(argv[5], &simulation->meal_count_max))
		return (write_error("number_of_times_each_philosopher_must_eat\n"),
			false);
	return (true);
}

bool	init_simul(t_simulation *simul)
{
	size_t	table_size;

	table_size = simul->number_of_philosophers * sizeof(t_philo);
	simul->philo_table = (t_philo *)malloc(table_size);
	if (!simul->philo_table)
		return (write_error("Can't allocate memory\n"), 1);
	memset(simul->philo_table, 0, table_size);
	simul->fork_sem = sem_open("/fork_sem", O_CREAT | O_EXCL, 0644,
			simul->number_of_philosophers);
	simul->state_sem = sem_open("/state_sem", O_CREAT | O_EXCL, 0644, 1);
	simul->death_sem = sem_open("/death_sem", O_CREAT | O_EXCL, 0644, 1);
	simul->report_sem = sem_open("/report_sem", O_CREAT | O_EXCL, 0644, 1);
	if (simul->fork_sem)
		sem_unlink("/fork_sem");
	if (simul->state_sem)
		sem_unlink("/state_sem");
	if (simul->death_sem)
		sem_unlink("/death_sem");
	if (simul->report_sem)
		sem_unlink("/report_sem");
	return (!(!simul->fork_sem || !simul->state_sem || !simul->death_sem
			|| !simul->report_sem));
}

bool	simul_launch(t_simulation *simul)
{
	pid_t	new_pid;
	int		i;

	sem_wait(simul->report_sem);
	simul->beginning_ts = get_current_timestamp();
	i = 0;
	while (i < simul->number_of_philosophers)
	{
		simul->philo_table[i].simul = simul;
		simul->philo_table[i].id = i + 1;
		new_pid = fork();
		simul->philo_table[i].pid = new_pid;
		if (new_pid == -1)
			return (write_error("Fork error\n"), false);
		if (new_pid == 0)
		{
			philo(&simul->philo_table[i]);
			destroy_simul(simul);
			exit (EXIT_SUCCESS);
		}
		i++;
	}
	simul->death_sem->__align = 0;
	return (sem_post(simul->report_sem), true);
}

bool	destroy_simul(t_simulation *simul)
{
	if (simul->fork_sem)
		sem_close(simul->fork_sem);
	if (simul->state_sem)
		sem_close(simul->state_sem);
	if (simul->death_sem)
		sem_close(simul->death_sem);
	if (simul->report_sem)
		sem_close(simul->report_sem);
	free(simul->philo_table);
	return (true);
}
