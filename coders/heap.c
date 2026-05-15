#include "codexion.h"


void heap_init(t_dongle *dongle, int capacity)
{
    dongle->waiters.data = malloc(sizeof(t_coder *) * capacity);
    dongle->waiters.capacity = capacity;
    dongle->waiters.size = 0;
}


int add_to_heap(t_heap *waiters, t_coder *coder)
{
    int index;

    index = waiters->size;
    if (waiters->size < waiters->capacity)
    {
        waiters->data[index] = coder;
        waiters->size++;
        return (1);
    }

    return (0);
}

t_coder	*pop_from_heap(t_heap *waiters)
{
	t_coder	*first;

	if (waiters->size == 0)
		return (NULL);

	first = waiters->data[0];

	if (waiters->size == 2)
		waiters->data[0] = waiters->data[1];

	waiters->data[1] = NULL;
	waiters->size--;

	return (first);
}


void	swap_heap(t_heap *waiters)
{
	t_coder	*tmp;
    // if (waiters->size != 2)
    //     return ;

	tmp = waiters->data[0];
	waiters->data[0] = waiters->data[1];
	waiters->data[1] = tmp;
}
