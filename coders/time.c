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