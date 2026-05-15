#include "codexion.h"

// int take_dongle(t_dongle *dongle, t_coder *coder)
// {
    
// }

// static int grab_dongles(t_coder *coder)
// {
//     t_dongle *first;
//     t_dongle *second;

//     if (coder->left->id < coder->right->id)
//     {
//         first = coder->left;
//         second = coder->right;
//     }
//     else
//     {
//         first = coder->right;
//         second = coder->left;
//     }

//     if (take_dongle(first, coder))
//         return (1);

//     print_msg(coder->info, coder->id, "has taken a dongle");

//     if (take_dongle(second, coder))
//     {
//         drop_dongle(first);
//         return (1);
//     }

//     print_msg(coder->info, coder->id, "has taken a dongle");

//     return (0);
// }


#include "codexion.h"

void	release_dongles(t_coder *coder)
{
	long	now;

	now = get_time_ms();

	pthread_mutex_lock(&coder->left->mutex);

	coder->left->taken = 0;
	coder->left->released_at = now;
	coder->left->available_at = now
		+ coder->info->dongle_cooldown;

	pthread_mutex_unlock(&coder->left->mutex);

	pthread_mutex_lock(&coder->right->mutex);

	coder->right->taken = 0;
	coder->right->released_at = now;
	coder->right->available_at = now + coder->info->dongle_cooldown;

	pthread_mutex_unlock(&coder->right->mutex);
}

static void wait_permition(t_coder *coder)
{
    pthread_mutex_lock(&coder->mutex);
    while (!coder->granted)
        pthread_cond_wait(&coder->cond, &coder->mutex);

    coder->granted = 0;
    pthread_mutex_unlock(&coder->mutex); 
}

void *coder_routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;
    if (coder->id % 2)
        usleep(500);

    while (!simulation_stopped(coder->info))
    {
        
        wait_permition(coder);        
        
        
        // after taking the s_dongle
        
        pthread_mutex_lock(&coder->mutex);
        coder->last_compile_start = get_time_ms();
        
        coder->compile_count++;
        
        if (coder->compile_count == coder->info->required_compiles)
        {
            pthread_mutex_lock(&coder->info->finish_mutex);
            coder->info->finished_coders++;
            pthread_mutex_unlock(&coder->info->finish_mutex);
        }
        pthread_mutex_unlock(&coder->mutex);
 
        
        
        log_state(coder, "is compiling");
        precise_sleep(coder->info->t_compile, coder->info);

        release_dongles(coder);

        log_state(coder, "is debugging");
        precise_sleep(coder->info->t_debug, coder->info);

        log_state(coder, "is refactoring");
        precise_sleep(coder->info->t_refactor, coder->info);
    }

    return (NULL);
}