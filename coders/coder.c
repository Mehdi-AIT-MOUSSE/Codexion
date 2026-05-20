#include "codexion.h"

// void	release_dongles(t_coder *coder)
// {
// 	long	now;

// 	now = get_time_ms();

// 	pthread_mutex_lock(&coder->left->mutex);

// 	coder->left->taken = 0;
// 	coder->left->released_at = now;

// 	pthread_mutex_unlock(&coder->left->mutex);

// 	pthread_mutex_lock(&coder->right->mutex);

// 	coder->right->taken = 0;
// 	coder->right->released_at = now;

// 	pthread_mutex_unlock(&coder->right->mutex);
// }

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
    while (!simulation_stopped(coder->info))
    {

        if (dongle->waiters.data[0] == coder && !dongle->taken)
        {
            // the fisrt issue you forget dongle_cooldown from parser !!
            cool_down = coder->info->dongle_cooldown - (get_time_ms() - dongle->released_at);

            if (cool_down <= 0)
                break ;
            
            pthread_mutex_unlock(&dongle->mutex);
            precise_sleep(cool_down, coder->info);     
            pthread_mutex_lock(&dongle->mutex);                        
            continue;
        }

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
    
    
    int finnish = 0;
    // avoid deadlock
    coder = (t_coder *)arg;
    

	// if (coder->id % 2)
    //     usleep(100);
    
    // if (coder->left->id < coder->right->id)
	if (coder->id % 2)
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
        
        if (!take_dongle(coder, first))
            return (NULL);


        
        if (!take_dongle(coder, second))
        {
            drop_dongle(first);
            return (NULL);
        }


        
        pthread_mutex_lock(&coder->mutex);
        
        log_state(coder, "is compiling");
        // after taking the dongles
        coder->last_compile_start = get_time_ms();        
        coder->compile_count++;
        
        if (coder->compile_count == coder->info->required_compiles)
        {
            pthread_mutex_lock(&coder->info->finish_mutex);
            coder->info->finished_coders++;
            pthread_mutex_unlock(&coder->info->finish_mutex);
            finnish = 1;
            coder->done = 1;
        }
        
        pthread_mutex_unlock(&coder->mutex);
        
        // Compiling
        precise_sleep(coder->info->t_compile, coder->info);

        // release_dongles;
        drop_dongle(first);
        drop_dongle(second);
        
        if (finnish)
        {
            log_state(coder, "is debugging");
            log_state(coder, "is refactoring");
        }
        
        if (simulation_stopped(coder->info))
            return (NULL);
        // Debugging & Refactoring
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







// void *coder_routine(void *arg)
// {
//     t_coder *coder;
//     int     finish;

//     coder = (t_coder *)arg;
//     finish = 0;
//     while (!simulation_stopped(coder->info))
//     {
//         if (is_done(coder))
//             return (NULL);

//         pthread_mutex_lock(&coder->left->mutex);
// 		add_to_heap(&coder->left->waiters, coder);
// 		pthread_mutex_unlock(&coder->left->mutex);

// 		pthread_mutex_lock(&coder->right->mutex);
// 		add_to_heap(&coder->right->waiters, coder);
// 		pthread_mutex_unlock(&coder->right->mutex);

        
//         pthread_mutex_lock(&coder->mutex);
        
//         // wait_permition(coder);
//         while (!coder->granted && !simulation_stopped(coder->info))
//             pthread_cond_wait(&coder->cond, &coder->mutex);
//         coder->granted = 0;
        
//         // after taking the dongle
//         coder->last_compile_start = get_time_ms();        
//         coder->compile_count++;
        
//         if (coder->compile_count == coder->info->required_compiles)
//         {
//             pthread_mutex_lock(&coder->info->finish_mutex);
//             coder->info->finished_coders++;
//             pthread_mutex_unlock(&coder->info->finish_mutex);
//             coder->done = 1;
//             finish = 1;
//         }

//         pthread_mutex_unlock(&coder->mutex);
        
//         log_state(coder, "is compiling");
//         precise_sleep(coder->info->t_compile, coder->info);

//         release_dongles(coder);

//         if (finish)
// 	        return (NULL);

//         if (simulation_stopped(coder->info))
// 	        return (NULL);
//         log_state(coder, "is debugging");
//         precise_sleep(coder->info->t_debug, coder->info);


//         if (simulation_stopped(coder->info))
// 	        return (NULL);
//         log_state(coder, "is refactoring");
//         precise_sleep(coder->info->t_refactor, coder->info);

//         if (simulation_stopped(coder->info))
//     	    return (NULL);
//     }

//     return (NULL);
// }
