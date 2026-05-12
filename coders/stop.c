#include "codexion.h"


int simulation_stopped(t_info *info)
{
    int stop;

    pthread_mutex_lock(&info->stop_mutex);
    stop = info->stop;
    pthread_mutex_unlock(&info->stop_mutex);

    return (stop);
}

void	stop_simulation(t_info *info)
{
    pthread_mutex_lock(&info->stop_mutex);
    info->stop = 1;
    pthread_mutex_unlock(&info->stop_mutex);    
}

void log_state(t_coder *coder, char *msg)
{
    pthread_mutex_lock(&coder->info->print_mutex);

    if (!simulation_stopped(coder->info))
        printf("%ld %d %s\n", timestamp_ms(coder->info), coder->id,msg);

    pthread_mutex_unlock(&coder->info->print_mutex);
}