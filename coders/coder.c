#include "codexion.h"

void drop_dongle(t_dongle *dongle)
{
    pthread_mutex_lock(&dongle->mutex);
    dongle->taken = 0;
    dongle->released_at = get_time_ms();
    pthread_cond_broadcast(&dongle->cond);
    pthread_mutex_unlock(&dongle->mutex);
}

int take_dongle(t_coder *coder, t_dongle *dongle)
{
    long cool_down;

    pthread_mutex_lock(&dongle->mutex);
    add_to_heap(&dongle->waiters, coder);
    check_preority(&dongle->waiters, coder->info->scheduler);
    while (!simulation_stopped(coder->info))
    {

        if (dongle->waiters.data[0] == coder && !dongle->taken)
        {
            cool_down = coder->info->dongle_cooldown - (get_time_ms() - dongle->released_at);

            if (cool_down <= 0)
                break ;
            
            pthread_mutex_unlock(&dongle->mutex);
            precise_sleep(cool_down, coder->info);     
            pthread_mutex_lock(&dongle->mutex);                        
        }
        else
            pthread_cond_wait(&dongle->cond, &dongle->mutex);
    }
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


void *coder_routine(void *arg)
{
    t_coder *coder;
    t_dongle	*first;
	t_dongle	*second;
    
    
    int coder_finnish = 0;
    coder = (t_coder *)arg;
    
    if (coder->left->id < coder->right->id)
	{
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
        pthread_mutex_lock(&coder->mutex);
        if (!take_dongle(coder, first))
        {
            pthread_mutex_unlock(&coder->mutex);            
            return (NULL);
        }
        
        if (coder->info->nb_coders == 1)
        {    
            pthread_mutex_unlock(&coder->mutex);
            return (NULL);
        }
        
        if (!take_dongle(coder, second))
        {
            pthread_mutex_unlock(&coder->mutex);
            drop_dongle(first);
            return (NULL);
        }
            
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
        
        // Compiling
        log_state(coder, "is compiling");
        precise_sleep(coder->info->t_compile, coder->info);

        drop_dongle(first);
        drop_dongle(second);

        if (coder_finnish || simulation_stopped(coder->info))
            return (NULL);
        
        // Debugging & Refactoring only if not done
        log_state(coder, "is debugging");
        precise_sleep(coder->info->t_debug, coder->info);
        
        if (simulation_stopped(coder->info))
            return (NULL);
        
        log_state(coder, "is refactoring");
        precise_sleep(coder->info->t_refactor, coder->info);
        
        if (simulation_stopped(coder->info))
            return (NULL);
            
        usleep(500);
    }
    
    return (NULL);
}
