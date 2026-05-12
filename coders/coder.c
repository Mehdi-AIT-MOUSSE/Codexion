#include "codexion.h"

void *coder_routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;

    while (!simulation_stopped(coder->info))
    {
        log_state(coder, "is compiling");

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

        // precise_sleep(coder->info->t_compile, coder->info);

        log_state(coder, "is debugging");

        // precise_sleep(coder->info->t_debug, coder->info);

        log_state(coder, "is refactoring");

        // precise_sleep(coder->info->t_refactor, coder->info);
    }

    return (NULL);
}