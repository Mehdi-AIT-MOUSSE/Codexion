#include "codexion.h"

long get_time_ms(void)
{
    struct timeval tv;
    
    if (gettimeofday(&tv, NULL) == -1)
    {
        write(2, "gettimeofday failed\n", 20);
        exit(1);
    }
    
    return ((long)tv.tv_sec * 1000L + (long)tv.tv_usec / 1000L);
}

long timestamp_ms(t_info *info)
{
    return (get_time_ms() - info->start_time);
}

void precise_sleep(long ms, t_info *info)
{
    long start;

    start = get_time_ms();

    while (!simulation_stopped(info))
    {
        if (get_time_ms() - start >= ms)
            break ;
        usleep(500);
    }
}
