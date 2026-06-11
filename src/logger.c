/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-mou <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 01:45:03 by mait-mou          #+#    #+#             */
/*   Updated: 2026/06/07 01:45:05 by mait-mou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	simulation_stopped(t_info *info)
{
	int	stop;

	pthread_mutex_lock(&info->stop_mutex);
	stop = info->stop;
	pthread_mutex_unlock(&info->stop_mutex);
	return (stop);
}

void	stop_simulation(t_info *info)
{
	int	i;

	pthread_mutex_lock(&info->stop_mutex);
	info->stop = 1;
	pthread_mutex_unlock(&info->stop_mutex);
	i = 0;
	while (i < info->nb_coders)
	{
		pthread_mutex_lock(&info->dongles[i].mutex);
		pthread_cond_broadcast(&info->dongles[i].cond);
		pthread_mutex_unlock(&info->dongles[i].mutex);
		i++;
	}
}

void	log_state(t_coder *coder, char *msg)
{
	long	time_ms;

	pthread_mutex_lock(&coder->info->print_mutex);
	time_ms = get_time_ms() - coder->info->start_time;
	if (!simulation_stopped(coder->info))
		printf("%ld %d %s\n", time_ms, coder->id, msg);
	pthread_mutex_unlock(&coder->info->print_mutex);
}

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long)tv.tv_sec * 1000L + (long)tv.tv_usec / 1000L);
}

void	precise_sleep(long ms, t_info *info)
{
	long	start;

	start = get_time_ms();
	while (!simulation_stopped(info))
	{
		if (get_time_ms() - start >= ms)
			break ;
		usleep(10);
	}
}
