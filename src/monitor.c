/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-mou <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 01:45:41 by mait-mou          #+#    #+#             */
/*   Updated: 2026/06/07 01:45:43 by mait-mou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_burnout(t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex);
	if (!coder->done)
	{
		if (get_time_ms() - coder->last_compile_start >= coder->info->t_burnout)
		{
			log_state(coder, "burned out");
			pthread_mutex_unlock(&coder->mutex);
			stop_simulation(coder->info);
			return (0);
		}
	}
	pthread_mutex_unlock(&coder->mutex);
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_info	*info;
	int		i;

	info = (t_info *)arg;
	while (!simulation_stopped(info))
	{
		i = 0;
		while (i < info->nb_coders)
		{
			if (!check_burnout(&info->coders[i]))
				return (NULL);
			pthread_mutex_lock(&info->finish_mutex);
			if (info->finished_coders >= info->nb_coders)
			{
				pthread_mutex_unlock(&info->finish_mutex);
				stop_simulation(info);
				return (NULL);
			}
			pthread_mutex_unlock(&info->finish_mutex);
			i++;
		}
		usleep(100);
	}
	return (NULL);
}
