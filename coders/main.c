#include "codexion.h"

int main(int argc, char **argv)
{
    t_info info;

    if (parse_args(argc, argv, &info))
        return (1);

    if (init_info(&info))
        return (1);

    start_simulation(&info);

    cleanup(&info);

    return (0);
}

