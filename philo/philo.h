
#ifndef PHILO_H
# define PHILO_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <sys/time.h>
# include <string.h>
# include <pthread.h>

typedef pthread_t			t_tid;
typedef pthread_mutex_t		t_mutex;
typedef unsigned long		t_timestamp;
typedef struct s_simulation	t_simulation;
typedef struct timeval		t_timeval;

typedef enum e_philo_state
{
	PHILO_STATE_PREPARE = 0,
	PHILO_STATE_THINKING = 1,
	PHILO_STATE_EATING = 2,
	PHILO_STATE_SLEEPING = 3,
	PHILO_STATE_DEAD = 4,
	PHILO_STATE_FULL = 5
}	t_philo_state;

typedef struct s_philo
{
	t_simulation	*simul;
	t_tid			thread_id;
	t_mutex			left_fork_mutex;
	t_mutex			state_mutex;
	t_timestamp		last_meal_timestamp;
	t_timestamp		be_nice;
	int				id;
	t_philo_state	state;
	int				meal_count;
	bool			init_fork_mutex_ok;
	bool			init_state_mutex_ok;
}	t_philo;

typedef enum e_simul_state
{
	SIMUL_RUNNING = 0,
	INIT_MUTEX = 1,
	INIT_MUTEXES = 2,
	INIT_THREADS = 3,
	STOPPED = 4,
}	t_simul_state;

typedef struct s_simulation
{
	t_mutex			report_mutex;
	t_philo			*philo_table;
	t_timestamp		beginning_ts;
	t_timestamp		time_to_die;
	t_timestamp		time_to_eat;
	t_timestamp		time_to_sleep;
	t_simul_state	simul_state;
	int				number_of_philosophers;
	int				meal_count_max;

	int				completely_full_count;
	bool			one_is_dead;
	bool			death_detected;
}	t_simulation;

t_timestamp	philo_report(t_philo *philo, t_timestamp timestamp,
				char *reportmsg);
t_timestamp	get_current_timestamp(void);
int			philo(t_philo *philo);
t_timestamp	philo_change_state(t_philo *philo, int new_state);
bool		init_simulation_params(t_simulation *simulation, int argc,
				char **argv);
bool		init_simulation(t_simulation *simul);
bool		init_simulation_threads(t_simulation *simul);
bool		is_simulation_finished(t_simulation *sim);
bool		is_arg_valid(char *arg_str, int *arg_val_ptr);
void		simul_clean(t_simulation *simul);
void		simul_join_threads(t_simulation *simul);
char		*parse_int(int *res_int, bool *res_valid, char *str);
void		write_error(char *msg);
void		philo_putback_forks(t_philo *philo);

#endif
