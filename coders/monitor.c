#include "codexion.h"

void *monitor_routine(void *arg)
{
    t_info  *info;
    int     i;
    int     finish;

    info = (t_info *)arg;

    while (!simulation_stopped(info))
    {
        // per-coder burnout check
        i = 0;
        while (i < info->nb_coders)
        {
            pthread_mutex_lock(&info->coders[i].mutex);
            if (!info->coders[i].done && get_time_ms() - info->coders[i].last_compile_start >= info->t_burnout)
            {
                log_state(&info->coders[i], "burned out");
                pthread_mutex_unlock(&info->coders[i].mutex);
                stop_simulation(info);
                return (NULL);
            }
            pthread_mutex_unlock(&info->coders[i].mutex);
            
            pthread_mutex_lock(&info->finish_mutex);
            finish = info->finished_coders;
            pthread_mutex_unlock(&info->finish_mutex);
    
            if (finish >= info->nb_coders)
            {
                stop_simulation(info);
                return (NULL);
            }

            i++;
        }

        // global finish check, once per pass

        usleep(500);
    }

    return (NULL);
}
