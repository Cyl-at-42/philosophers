
#include "philo_bonus.h"

char	*parse_int(int *res_int, bool *res_valid, char *str)
{
	unsigned int	nbr;
	char			*in_str;
	char			*msd_str;

	nbr = 0;
	in_str = str;
	str += (*str == '-' || *str == '+');
	*res_valid = (*str == '0');
	while (*str == '0')
		str++;
	if ((*str < '0' || *str > '9') && !*res_valid)
		return (in_str);
	msd_str = str;
	while (*str >= '0' && *str <= '9')
		nbr = nbr * 10 + (*str++ - '0');
	if (((str - msd_str) > 10) || ((str - msd_str) == 10 && *msd_str >= '4'))
		return (*res_valid = 0, str);
	if ((nbr == 2147483648) && (*in_str == '-'))
		return (*res_int = 0x80000000, *res_valid = 1, str);
	*res_valid = (nbr < 2147483648);
	return (*res_int = (int)nbr * (1 - ((*in_str == '-') << 1)), str);
}

t_timestamp	get_current_timestamp(void)
{
	t_timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000000 + time.tv_usec);
}

void	write_error(char *msg)
{
	char	*msg2;

	if (!msg)
		return ;
	if (!*msg)
		return ;
	msg2 = msg;
	while (*msg2++)
		;
	write(2, msg, msg2 - msg - 1);
}

void	print_help(void)
{
	printf("Usage:\n-----------\n");
	printf("philo_bonus number_of_philosophers time_to_die time_to_eat");
	printf("time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
}

int	main(int argc, char **argv)
{
	t_simulation	simul;
	int				i;

	if (argc < 5 || argc > 6)
		return (print_help(), 1);
	if (!init_simulation_params(&simul, argc, argv))
		return (1);
	if (!init_simul(&simul))
		return (destroy_simul(&simul), 0);
	simul_launch(&simul);
	i = 0;
	while (i < simul.number_of_philosophers)
	{
		if (simul.philo_table[i].pid > 0)
			waitpid(simul.philo_table[i].pid, NULL, 0);
		i++;
	}
	destroy_simul(&simul);
	return (0);
}
