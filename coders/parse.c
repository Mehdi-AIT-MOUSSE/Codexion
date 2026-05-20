#include "codexion.h"

static void fill_the_info(char **argv, t_info *info)
{
    info->nb_coders     = atoi(argv[1]);
    info->t_burnout     = (long)atoi(argv[2]);
    info->t_compile     = (long)atoi(argv[3]);
    info->t_debug       = (long)atoi(argv[4]);
    info->t_refactor    = (long)atoi(argv[5]);
    info->required_compiles = atoi(argv[6]);
    info->dongle_cooldown     = (long)atoi(argv[7]);
    if (!strcmp(argv[8], "fifo"))
        info->scheduler = FIFO;
    else
        info->scheduler = EDF;


    info->stop = 0;
    info->finished_coders = 0;
}

int parse_args(int argc, char **argv, t_info *info)
{
    int i;
    int c;

    if (argc != 9)
        return (write(2, "Error: All arguments are mandatory\n", 35), 1);
    i = 1;
    while (i <= 7)
    {
        if (argv[i][0] == '\0')
            return (write(2, "Error: empty argument\n", 22), 1);
        c = 0;
        while (argv[i][c])
        {
            if (argv[i][c] < '0' || argv[i][c] > '9')
                return (write(2, "Error: first 7 args must be positive integers\n", 46), 1);
            c++;
        }
        if (atoi(argv[i]) <= 0 && i != 7)
            return (write(2, "Error: arguments must be > 0\n", 29), 1);
        i++;
    }
    if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
        return (write(2, "Error: scheduler must be fifo or edf\n", 37), 1);
    fill_the_info(argv, info);
    return (0);
}