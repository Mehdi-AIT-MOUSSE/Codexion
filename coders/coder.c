/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-mou <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 01:43:04 by mait-mou          #+#    #+#             */
/*   Updated: 2026/06/07 01:43:10 by mait-mou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	handle_compile(t_coder *coder)
{
	int	coder_finnish;

	coder_finnish = 0;
	pthread_mutex_lock(&coder->mutex);
	coder->last_compile_start = get_time_ms();
	coder->compile_count++;
	if (coder->compile_count == coder->info->required_compiles)
	{
		pthread_mutex_lock(&coder->info->finish_mutex);
		coder->info->finished_coders++;
		pthread_mutex_unlock(&coder->info->finish_mutex);
		coder_finnish = 1;
		coder->done = 1;
	}
	pthread_mutex_unlock(&coder->mutex);
	return (coder_finnish);
}

static int	do_cycle(t_coder *coder, t_dongle *first, t_dongle *second)
{
	int	coder_finnish;

	if (!take_dongle(coder, first))
		return (-1);
	if (!take_dongle(coder, second))
	{
		drop_dongle(first);
		return (-1);
	}
	coder_finnish = handle_compile(coder);
	log_state(coder, "is compiling");
	precise_sleep(coder->info->t_compile, coder->info);
	drop_dongle(first);
	drop_dongle(second);
	if (coder_finnish || simulation_stopped(coder->info))
		return (0);
	log_state(coder, "is debugging");
	precise_sleep(coder->info->t_debug, coder->info);
	if (simulation_stopped(coder->info))
		return (0);
	log_state(coder, "is refactoring");
	precise_sleep(coder->info->t_refactor, coder->info);
	return (1);
}

void	*coder_routine(void *arg)
{
	t_coder		*coder;
	t_dongle	*first;
	t_dongle	*second;
	int			ret;

	coder = (t_coder *)arg;
	if (coder->id % 2 == 0)
	{
		usleep(10);
		first = coder->left;
		second = coder->right;
	}
	else
	{
		first = coder->right;
		second = coder->left;
	}
	while (!simulation_stopped(coder->info))
	{
		ret = do_cycle(coder, first, second);
		if (ret <= 0)
			return (NULL);
		usleep(100);
	}
	return (NULL);
}
