#include "codexion.h"

void *monitor_routine(void *arg)
{
    t_info  *info;
    int     i;

    info = (t_info *)arg;

    while (!simulation_stopped(info))
    {
        i = 0;

        while (i < info->nb_coders)
        {
            pthread_mutex_lock(&info->coders[i].mutex);

            if (get_time_ms() - info->coders[i].last_compile_start > info->t_burnout)
            {
                pthread_mutex_unlock(&info->coders[i].mutex);
                log_state(&info->coders[i], "burned out");
                stop_simulation(info);
                return (NULL);
            }

            pthread_mutex_unlock(&info->coders[i].mutex);

            i++;
        }

        pthread_mutex_lock(&info->finish_mutex);
        if (info->finished_coders == info->nb_coders)
        {
            stop_simulation(info);
            return (NULL);
        }
        pthread_mutex_unlock(&info->finish_mutex);

        usleep(1000);
    }

    return (NULL);
}