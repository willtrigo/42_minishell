/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 03:19:01 by dande-je          #+#    #+#             */
/*   Updated: 2024/11/26 01:35:55 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include "ft_string.h"
#include "internal/default.h"
#include "internal/env/envp.h"
#include "internal/repl/shell/runner/builtins/builtins.h"

static t_builtins	builtins_init(void);
static sig_atomic_t	return_buildin_status(t_builtin_id builtin, \
						sig_atomic_t status, t_runner_data *run_data);

t_builtin_id	check_builtin(t_command cmd)
{
	int					builtins_len;
	const t_builtins	builtins = builtins_init();
	char				*invocation;

	builtins_len = DEFAULT_INIT;
	if (cmd->type != CMD_SIMPLE || cmd->simple->cmd_argc <= DEFAULT)
		return (NOT_BUILTIN);
	invocation = cmd->simple->cmd_argv[DEFAULT];
	while (builtins.builtin[++builtins_len].invocation)
	{
		if (ft_strncmp(invocation, \
			builtins.builtin[builtins_len].invocation, \
			ft_strlen(builtins.builtin[builtins_len].invocation) \
			+ NULL_BYTE) == DEFAULT)
			return (builtins.builtin[builtins_len].id);
	}
	return (NOT_BUILTIN);
}

sig_atomic_t	runner_cmd_builtin(t_builtin_id builtin, t_command cmd)
{
	int					builtins_len;
	sig_atomic_t		status;
	const t_builtins	builtins = builtins_init();

	builtins_len = DEFAULT_INIT;
	status = EXIT_OK;
	while (builtins.builtin[++builtins_len].invocation)
	{
		if (builtin != builtins.builtin[builtins_len].id)
			continue ;
		status = builtins.builtin[builtins_len].fn(cmd);
		break ;
	}
	return (status);
}

sig_atomic_t	runner_cmd_builtin_without_fork(t_builtin_id builtin, \
					t_runner_data *run_data)
{
	int				copy_fds[FD_SIZE];
	sig_atomic_t	status;
	char			*err_msg;

	copy_fds[FD_IN] = dup(STDIN_FILENO);
	copy_fds[FD_OUT] = dup(STDOUT_FILENO);
	run_data->cmd->simple->cmd_envp = get_envp(ENVP_DEFAULT);
	if (!io_handlers_redirect(run_data->cmd->io_handlers))
	{
		err_msg = io_handlers_get_error(run_data->cmd->io_handlers);
		dup2(copy_fds[FD_IN], STDIN_FILENO);
		dup2(copy_fds[FD_OUT], STDOUT_FILENO);
		close(copy_fds[FD_IN]);
		close(copy_fds[FD_OUT]);
		runner_cmd_simple_panic(
			run_data, ft_strdup(err_msg), EXIT_REDIRECT_FAIL, false);
		return (EXIT_REDIRECT_FAIL);
	}
	status = runner_cmd_builtin(builtin, run_data->cmd);
	dup2(copy_fds[FD_IN], STDIN_FILENO);
	dup2(copy_fds[FD_OUT], STDOUT_FILENO);
	close(copy_fds[FD_IN]);
	close(copy_fds[FD_OUT]);
	close_fd_lst(run_data->pipes_to_close);
	return (return_buildin_status(builtin, status, run_data));
}

static sig_atomic_t	return_buildin_status(t_builtin_id builtin, \
						sig_atomic_t status, t_runner_data *run_data)
{
	print_builtin_exit(builtin, run_data, status);
	if (builtin == BUILTIN_EXIT \
		&& (status == EXIT_NOT_NUMERIC || status != EXIT_TOO_MANY))
	{
		if (status < 0)
			status *= -1;
		runner_cmd_simple_exit_status(run_data, status);
	}
	if (status < 0)
		status *= -1;
	return (status);
}

static t_builtins	builtins_init(void)
{
	return ((t_builtins){.builtin = {
			{BUILTIN_ECHO, "echo", &runner_cmd_builtin_echo},
			{BUILTIN_EXPORT, "export", &runner_cmd_builtin_export},
			{BUILTIN_UNSET, "unset", &runner_cmd_builtin_unset},
			{BUILTIN_ENV, "env", &runner_cmd_builtin_env},
			{BUILTIN_EXIT, "exit", &runner_cmd_builtin_exit},
			{BUILTIN_PWD, "pwd", &runner_cmd_builtin_pwd},
			{BUILTIN_CD, "cd", &runner_cmd_builtin_cd},
			{NOT_BUILTIN, NULL, NULL},
		}});
}
