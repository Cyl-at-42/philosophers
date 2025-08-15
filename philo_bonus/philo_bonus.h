
#ifndef PHILO_BONUS_H
# define PHILO_BONUS_H

# include <signal.h>
# include <limits.h>
# include <semaphore.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <sys/time.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <string.h>
# include <pthread.h>

typedef pid_t				t_pid;
typedef pthread_t			t_tid;
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
	t_pid			pid;
	t_timestamp		last_meal_timestamp;
	t_timestamp		be_nice;
	int				id;
	t_philo_state	state;
	int				meal_count;
}	t_philo;

typedef struct s_simulation
{
	sem_t			*fork_sem;
	sem_t			*report_sem;
	sem_t			*death_sem;
	sem_t			*state_sem;
	t_philo			*philo_table;
	t_timestamp		beginning_ts;
	t_timestamp		time_to_die;
	t_timestamp		time_to_eat;
	t_timestamp		time_to_sleep;
	int				number_of_philosophers;
	int				meal_count_max;
}	t_simulation;

void		*philo_thread(void *ptr);
t_timestamp	get_current_timestamp(void);
bool		init_simul(t_simulation *simul);
bool		init_simulation_params(t_simulation *simulation, int argc,
				char **argv);
void		write_error(char *msg);
int			philo(t_philo *philo);
char		*parse_int(int *res_int, bool *res_valid, char *str);
t_timestamp	philo_report(t_philo *philo, t_timestamp timestamp,
				char *reportmsg);
t_timestamp	philo_change_state(t_philo *philo, int new_state);
bool		simul_launch(t_simulation *simul);
bool		destroy_simul(t_simulation *simul);

#endif
