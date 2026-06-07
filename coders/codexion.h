/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-mou <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 01:43:50 by mait-mou          #+#    #+#             */
/*   Updated: 2026/06/07 01:43:54 by mait-mou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <string.h>

# define FIFO 0
# define EDF 1

typedef struct s_info	t_info;
typedef struct s_coder	t_coder;
typedef struct s_dongle	t_dongle;
typedef struct s_heap	t_heap;

struct s_heap
{
	t_coder	**data;
	int		size;
	int		capacity;
};

struct s_dongle
{
	int				id;
	int				taken;
	long			released_at;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	t_heap			waiters;
};

struct s_coder
{
	int				id;
	int				compile_count;
	long			last_compile_start;
	long			done;
	pthread_t		thread;
	pthread_mutex_t	mutex;
	t_dongle		*left;
	t_dongle		*right;
	t_info			*info;
};

struct s_info
{
	int				nb_coders;
	int				t_burnout;
	int				t_compile;
	int				t_debug;
	int				t_refactor;
	long			dongle_cooldown;
	int				required_compiles;
	int				stop;
	long			start_time;
	int				scheduler;
	int				finished_coders;
	pthread_t		monitor;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	finish_mutex;
	t_dongle		*dongles;
	t_coder			*coders;
};

int		parse_args(int argc, char **argv, t_info *info);
int		init_info(t_info *info);
int		start_simulation(t_info *info);
int		take_dongle(t_coder *coder, t_dongle *dongle);
void	drop_dongle(t_dongle *dongle);
void	*coder_routine(void *arg);
void	*monitor_routine(void *arg);
long	get_time_ms(void);
void	precise_sleep(long ms, t_info *info);
void	log_state(t_coder *coder, char *msg);
int		simulation_stopped(t_info *info);
void	stop_simulation(t_info *info);
void	heap_init(t_dongle *dongle, int capacity);
int		add_to_heap(t_heap *waiters, t_coder *coder);
void	pop_from_heap(t_heap *waiters);
void	swap_heap(t_heap *waiters);
void	check_preority(t_heap *waiters, int scheduler);
void	cleanup(t_info *info);

#endif
