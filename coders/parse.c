/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-mou <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 01:45:59 by mait-mou          #+#    #+#             */
/*   Updated: 2026/06/07 01:46:01 by mait-mou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	fill_the_info(char **argv, t_info *info)
{
	info->nb_coders = atoi(argv[1]);
	info->t_burnout = (long)atoi(argv[2]);
	info->t_compile = (long)atoi(argv[3]);
	info->t_debug = (long)atoi(argv[4]);
	info->t_refactor = (long)atoi(argv[5]);
	info->required_compiles = atoi(argv[6]);
	info->dongle_cooldown = (long)atoi(argv[7]);
	if (!strcmp(argv[8], "fifo"))
		info->scheduler = FIFO;
	else
		info->scheduler = EDF;
	info->stop = 0;
	info->finished_coders = 0;
}

static int	is_int_overflow(char *str)
{
	long	val;
	int		i;

	val = 0;
	i = 0;
	while (str[i])
	{
		val = val * 10 + (str[i] - '0');
		if (val > 2147483647)
			return (1);
		i++;
	}
	return (0);
}

static int	check_arg(char *arg, int i)
{
	int	c;

	if (arg[0] == '\0')
		return (write(2, "Error: empty argument\n", 22), 1);
	c = 0;
	while (arg[c])
	{
		if (arg[c] < '0' || arg[c] > '9')
			return (write(2,
					"Error: first 7 args must be positive integers\n",
					46), 1);
		c++;
	}
	if (is_int_overflow(arg))
		return (write(2, "Error: argument exceeds INT_MAX\n", 32), 1);
	if (atoi(arg) <= 0 && i != 7)
		return (write(2, "Error: arguments must be > 0\n", 29), 1);
	return (0);
}

int	parse_args(int argc, char **argv, t_info *info)
{
	int	i;

	if (argc != 9)
		return (write(2, "Error: All arguments are mandatory\n", 35), 1);
	i = 1;
	while (i <= 7)
	{
		if (check_arg(argv[i], i))
			return (1);
		i++;
	}
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
		return (write(2, "Error: scheduler must be fifo or edf\n", 37), 1);
	fill_the_info(argv, info);
	return (0);
}
