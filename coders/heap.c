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

void	pop_from_heap(t_heap *waiters)
{
	if (waiters->size == 0)
		return ;

	else if (waiters->size == 1)
		waiters->data[0] = NULL;

	else
	{
		waiters->data[0] = waiters->data[1];
		waiters->data[1] = NULL;
	}

	waiters->size--;
}

void	swap_heap(t_heap *waiters)
{
	t_coder	*tmp;

	tmp = waiters->data[0];
	waiters->data[0] = waiters->data[1];
	waiters->data[1] = tmp;
}

void	check_preority(t_heap *waiters, int scheduler)
{
	t_coder *a;
	t_coder *b;
	long	first_deadline;
	long	second_deadline;

	if (waiters->size != 2 || scheduler == FIFO)
		return ;

	a = waiters->data[0];
	b = waiters->data[1];

	pthread_mutex_lock(&a->mutex);
    first_deadline = a->last_compile_start + a->info->t_burnout;
	pthread_mutex_unlock(&a->mutex);
    
	pthread_mutex_lock(&b->mutex);
	second_deadline = b->last_compile_start + b->info->t_burnout;
	pthread_mutex_unlock(&b->mutex);

	if (second_deadline < first_deadline)
		swap_heap(waiters);
}