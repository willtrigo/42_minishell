/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   runner_data.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 09:39:03 by maurodri          #+#    #+#             */
/*   Updated: 2024/10/17 16:13:23 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "collection/ft_arraylist.h"
#include "ft_stdio.h"
#include "runner.h"

void runner_data_init(
	t_runner_data *run_data, t_command cmd, sig_atomic_t last_cmd_status)
{
	run_data->last_cmd_status = last_cmd_status;
	run_data->base_cmd = cmd;
	run_data->cmd = cmd;
	run_data->pids = ft_arraylist_new(free);
	run_data->pipes_to_close = ft_arraylist_new(free);
	run_data->backup_pids = NULL;
}

void runner_data_clean(t_runner_data *run_data)
{
	ft_arraylist_destroy(run_data->pids);
	ft_arraylist_destroy(run_data->pipes_to_close);
	if (run_data->backup_pids)
		ft_arraylist_destroy(run_data->backup_pids);
	command_destroy(run_data->base_cmd);
}
