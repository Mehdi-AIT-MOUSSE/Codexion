#include "codexion.h"

void *monitor_routine(void *arg)
{
    t_info  *info;
    int     i;
    int     finish;

    info = (t_info *)arg;

    while (!simulation_stopped(info))
    {
        i = 0;

        while (i < info->nb_coders)
        {
            
            
            pthread_mutex_lock(&info->coders[i].mutex);
            if (get_time_ms() - info->coders[i].last_compile_start >= info->t_burnout)
            {
                log_state(&info->coders[i], "burned out");
                printf("burned out -> %ld\n", get_time_ms() - info->coders[i].last_compile_start);

                pthread_mutex_unlock(&info->coders[i].mutex);
                stop_simulation(info);
                return (NULL);
            }
            
            pthread_mutex_lock(&info->finish_mutex);
            finish = info->finished_coders;
            pthread_mutex_unlock(&info->finish_mutex);
            
            if (finish >= info->nb_coders)
            {
                stop_simulation(info);
                return (NULL);
            }
            
            pthread_mutex_unlock(&info->coders[i].mutex);
            i++;
        }
        
        usleep(500);

    }

    return (NULL);
}
