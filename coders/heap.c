#include "codexion.h"


void heap_init(t_dongle *dongle, int capacity)
{
    dongle->waiters.data = malloc(sizeof(t_coder *) * capacity);
    dongle->waiters.capacity = capacity;
    dongle->waiters.size = 0;
}


// int add_to_heap(t_heap *waiters, t_coder *coder)
// {
//     int index;

//     index = waiters->size;
//     if (waiters->size < waiters->capacity)
//     {
//         waiters->data[index] = coder;
//         waiters->size++;
//         return (1);
//     }

//     return (0);
// }

int add_to_heap(t_heap *waiters, t_coder *coder)
{
    int index;

    index = waiters->size;

    if (waiters->size >= 1 && waiters->data[0] == coder)
        return (1);
    
    if (waiters->size == 2 && waiters->data[1] == coder)
        return (1);
    
    if (waiters->size >= 2)
        return (0);
    
    waiters->data[index] = coder;
    waiters->size++;

    return (1);
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

void	remove_from_heap(t_heap *waiters, t_coder *coder)
{
	if (waiters->size == 0)
		return ;

	/*
	** coder at front
	*/
	if (waiters->data[0] == coder)
	{
		pop_from_heap(waiters);
		return ;
	}

	/*
	** coder at second position
	*/
	if (waiters->size == 2
		&& waiters->data[1] == coder)
	{
		waiters->data[1] = NULL;
		waiters->size--;
	}
}

t_coder	*heap_top(t_heap *waiters)
{
	if (waiters->size == 0)
		return (NULL);

	return (waiters->data[0]);
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
