#include "codexion.h"

void	release_dongles(t_coder *coder)
{
	long	now;

	now = get_time_ms();

	pthread_mutex_lock(&coder->left->mutex);

	coder->left->taken = 0;
	coder->left->released_at = now;
	coder->left->available_at = now + coder->info->dongle_cooldown;

	pthread_mutex_unlock(&coder->left->mutex);

	pthread_mutex_lock(&coder->right->mutex);

	coder->right->taken = 0;
	coder->right->released_at = now;
	coder->right->available_at = now + coder->info->dongle_cooldown;

	pthread_mutex_unlock(&coder->right->mutex);
}

static int is_done(t_coder *coder)
{
    int done;

    pthread_mutex_lock(&coder->mutex);
    done = coder->done;
    pthread_mutex_unlock(&coder->mutex);

    return (done);
}

void *coder_routine(void *arg)
{
    t_coder *coder;
    int     finish;

    coder = (t_coder *)arg;
    finish = 0;
    while (!simulation_stopped(coder->info))
    {
        if (is_done(coder))
            return (NULL);

        pthread_mutex_lock(&coder->left->mutex);
		add_to_heap(&coder->left->waiters, coder);
		pthread_mutex_unlock(&coder->left->mutex);

		pthread_mutex_lock(&coder->right->mutex);
		add_to_heap(&coder->right->waiters, coder);
		pthread_mutex_unlock(&coder->right->mutex);

        
        pthread_mutex_lock(&coder->mutex);
        
        // wait_permition(coder);
        while (!coder->granted && !simulation_stopped(coder->info))
            pthread_cond_wait(&coder->cond, &coder->mutex);
        coder->granted = 0;
        
        // after taking the dongle
        coder->last_compile_start = get_time_ms();        
        coder->compile_count++;
        
        if (coder->compile_count == coder->info->required_compiles)
        {
            pthread_mutex_lock(&coder->info->finish_mutex);
            coder->info->finished_coders++;
            pthread_mutex_unlock(&coder->info->finish_mutex);
            coder->done = 1;
            finish = 1;
        }

        pthread_mutex_unlock(&coder->mutex);
        
        log_state(coder, "is compiling");
        precise_sleep(coder->info->t_compile, coder->info);

        release_dongles(coder);

        if (finish)
	        return (NULL);

        if (simulation_stopped(coder->info))
	        return (NULL);
        log_state(coder, "is debugging");
        precise_sleep(coder->info->t_debug, coder->info);


        if (simulation_stopped(coder->info))
	        return (NULL);
        log_state(coder, "is refactoring");
        precise_sleep(coder->info->t_refactor, coder->info);

        if (simulation_stopped(coder->info))
    	    return (NULL);
    }

    return (NULL);
}








