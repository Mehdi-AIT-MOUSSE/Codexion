#include "codexion.h"\

/*
** both dongles available ?
*/
static int	can_grant(t_coder *coder)
{
	long	now;

	now = get_time_ms();

	// are dongles are free
	if (coder->left->taken)
		return (0);

	if (coder->right->taken)
		return (0);

	// check dongle_cooldown
	if (now < coder->left->available_at)
		return (0);

	if (now < coder->right->available_at)
		return (0);

	// is_first_waiter
	if (heap_top(&coder->left->waiters) != coder)
		return (0);

	if (heap_top(&coder->right->waiters) != coder)
		return (0);

	return (1);
}

/*
** scheduler grants BOTH dongles atomically
*/
static void	grant_dongles(t_coder *coder)
{
	coder->left->taken = 1;
	log_state(coder, "has taken a dongle");

	coder->right->taken = 1;
	log_state(coder, "has taken a dongle");

	remove_from_heap(&coder->left->waiters, coder);
	remove_from_heap(&coder->right->waiters, coder);

	pthread_mutex_lock(&coder->mutex);
	coder->granted = 1;
	pthread_cond_broadcast(&coder->cond);
	pthread_mutex_unlock(&coder->mutex);
}

/*
** try grant compile permission
*/
void	try_grant(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->left->id < coder->right->id)
	{
		first = coder->left;
		second = coder->right;
	}
	else
	{
		first = coder->right;
		second = coder->left;
	}

	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);

	if (can_grant(coder))
		grant_dongles(coder);

	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
}

/*
** central scheduler thread
*/
// void	*scheduler_routine(void *arg)
// {
// 	t_info	*info;
// 	int		i;
// 	info = (t_info *)arg;
// 	while (!simulation_stopped(info))
// 	{
// 		i = 0;
// 		while (i < info->nb_coders)
// 		{
// 			try_grant(&info->coders[i]);
// 			i++;
// 		}
// 		usleep(500);
// 	}
// 	return (NULL);
// }