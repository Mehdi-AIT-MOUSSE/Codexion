/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-mou <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 01:45:24 by mait-mou          #+#    #+#             */
/*   Updated: 2026/06/07 01:45:30 by mait-mou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_info	info;

	if (parse_args(argc, argv, &info))
		return (1);
	if (init_info(&info))
		return (1);
	start_simulation(&info);
	cleanup(&info);
	return (0);
}
