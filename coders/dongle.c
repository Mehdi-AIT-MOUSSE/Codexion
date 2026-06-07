/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-mou <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 01:44:17 by mait-mou          #+#    #+#             */
/*   Updated: 2026/06/07 01:44:19 by mait-mou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	drop_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->taken = 0;
	dongle->released_at = get_time_ms();
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

static void	wait_for_dongle(t_coder *coder, t_dongle *dongle)
{
	long	cool_down;

	while (!simulation_stopped(coder->info))
	{
		if (dongle->waiters.data[0] == coder && !dongle->taken)
		{
			cool_down = coder->info->dongle_cooldown
				- (get_time_ms() - dongle->released_at);
			if (cool_down <= 0)
				break ;
			pthread_mutex_unlock(&dongle->mutex);
			precise_sleep(cool_down, coder->info);
			pthread_mutex_lock(&dongle->mutex);
		}
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
}

int	take_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	add_to_heap(&dongle->waiters, coder);
	pthread_mutex_unlock(&dongle->mutex);
	usleep(500);
	pthread_mutex_lock(&dongle->mutex);
	check_preority(&dongle->waiters, coder->info->scheduler);
	wait_for_dongle(coder, dongle);
	if (simulation_stopped(coder->info))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	pop_from_heap(&dongle->waiters);
	dongle->taken = 1;
	log_state(coder, "has taken a dongle");
	pthread_mutex_unlock(&dongle->mutex);
	return (1);
}
