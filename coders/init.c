#include "codexion.h"

static int		init_coders(t_info *info)
{
    int i;

    info->coders = malloc(sizeof(t_coder) * info->nb_coders);
    if (!info->coders)
        return (1);

    i = 0;
    while (i < info->nb_coders)
    {
        info->coders[i].id = i + 1;
        
        info->coders[i].compile_count = 0;
        
        info->coders[i].last_compile_start = info->start_time;
        
        info->coders[i].request_time = 0;
        
        info->coders[i].granted = 0;

        info->coders[i].left = &info->dongles[i];
        
        info->coders[i].right = &info->dongles[(i + 1) % info->nb_coders];
        
        info->coders[i].info = info;
        
        pthread_mutex_init(&info->coders[i].mutex,NULL);
        pthread_cond_init(&info->coders[i].cond, NULL);

        i++;
    }

    return (0);
}

static int		init_dongles(t_info *info)
{
    int i;

    info->dongles = malloc(sizeof(t_dongle) * info->nb_coders);
    if (!info->dongles)
        return (1);

    i = 0;
    while (i < info->nb_coders)
    {
        info->dongles[i].id = i + 1;

        info->dongles[i].taken = 0;

        info->dongles[i].available_at = 0;

        info->dongles[i].released_at = 0;

        heap_init(&info->dongles[i], 2);

        pthread_mutex_init(&info->dongles[i].mutex, NULL);


        i++;
    }

    return (0);
}

int		init_info(t_info *info)
{
    pthread_mutex_init(&info->stop_mutex, NULL);
    pthread_mutex_init(&info->print_mutex, NULL);
    pthread_mutex_init(&info->finish_mutex, NULL);

    if (init_dongles(info))
        return (1);

    if (init_coders(info))
        return (1);

    return (0);    
}

int start_simulation(t_info *info)
{
    int i;

    i = 0;
    while (i < info->nb_coders)
    {
        pthread_create(&info->coders[i].thread,NULL,coder_routine,&info->coders[i]);
        i++;
    }

    pthread_create(&info->monitor,NULL,monitor_routine,info);

    i = 0;
    while (i < info->nb_coders)
    {
        pthread_join(info->coders[i].thread, NULL);
        i++;
    }

    pthread_join(info->monitor, NULL);

    return (0);
}

void cleanup(t_info *info)
{
    int i;

    i = 0;

    while (i < info->nb_coders)
    {
        pthread_mutex_destroy(&info->dongles[i].mutex);
        free(info->dongles[i].waiters.data);

        pthread_mutex_destroy(&info->coders[i].mutex);
        pthread_cond_destroy(&info->coders[i].cond);
        
        i++;
    }


    pthread_mutex_destroy(&info->stop_mutex);

    pthread_mutex_destroy(&info->print_mutex);

    pthread_mutex_destroy(&info->finish_mutex);

    free(info->dongles);

    free(info->coders);
}

