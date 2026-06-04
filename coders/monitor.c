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
			{
				stop_simulation(info);
				return (NULL);
			}
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
		usleep(500);
	}
	return (NULL);
}