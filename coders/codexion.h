#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <string.h>

#define FIFO 0
#define EDF 1

typedef struct s_info		t_info;
typedef struct s_coder		t_coder;
typedef struct s_dongle		t_dongle;
typedef struct s_heap		t_heap;

/*
** Priority queue / heap
*/
struct s_heap
{
	t_coder	**data;
	int		size;
	int		capacity;
};

/*
** Dongle
*/
struct s_dongle
{
	int					id;
	int					taken;

	long				released_at;

	pthread_mutex_t		mutex;
	pthread_cond_t		cond;
		
	t_heap				waiters;
};

/*
** Coder
*/
struct s_coder
{
	int					id;
	
	int					compile_count;
	
	long				last_compile_start;
	long				done;
	
	pthread_t			thread;
    pthread_mutex_t     mutex;

	t_dongle			*left;
	t_dongle			*right;

	t_info				*info;
};

/*
** Global simulation info
*/
struct s_info
{
	int					nb_coders;

	long				t_burnout;
	long				t_compile;
	long				t_debug;
	long				t_refactor;

	long				dongle_cooldown;

	int					required_compiles;

	int					stop;

	long				start_time;

	int 				scheduler;

    int                 finished_coders;

	pthread_t			monitor;
	pthread_t			scheduler_thread;

	pthread_mutex_t		stop_mutex;
	pthread_mutex_t		print_mutex;
    pthread_mutex_t     finish_mutex;

	t_dongle			*dongles;
	t_coder				*coders;
};

/*
** Parsing
*/
int parse_args(int argc, char **argv, t_info *info);

/*
** Initialization
*/
int		init_info(t_info *info);
// int		init_dongles(t_info *info);
// int		init_coders(t_info *info);

int start_simulation(t_info *info); // start_simulation.c

/*
** Threads
*/
void	*coder_routine(void *arg);
void	*scheduler_routine(void *arg);
void	*monitor_routine(void *arg);

// coder
void	release_dongles(t_coder *coder);

/*
** Time
*/
long	get_time_ms(void);  //parse.c
long	timestamp_ms(t_info *info); //log.c

/*
** Sleep
*/
void	precise_sleep(long ms, t_info *info);

/*
** Logging
*/
void	log_state(t_coder *coder, char *msg);

/*
** Stop handling
*/
int		simulation_stopped(t_info *info);
void	stop_simulation(t_info *info);

// scheduler_routine
void	try_grant(t_coder *coder);


/*
** Dongles
*/
// int		take_dongles(t_coder *coder);
// void	release_dongles(t_coder *coder);

/*
** Heap / Queue
*/
void heap_init(t_dongle *dongle, int capacity);
int add_to_heap(t_heap *waiters, t_coder *coder);
void	pop_from_heap(t_heap *waiters);
void	remove_from_heap(t_heap *waiters, t_coder *coder);
t_coder	*heap_top(t_heap *waiters);
void	swap_heap(t_heap *waiters);


/*
** Cleanup
*/
void	cleanup(t_info *info);

#endif