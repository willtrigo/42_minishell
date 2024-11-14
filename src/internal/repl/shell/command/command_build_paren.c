/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_build_paren.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 22:05:18 by maurodri          #+#    #+#             */
/*   Updated: 2024/11/14 00:58:46 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "collection/ft_arraylist.h"
#include "ft_stdio.h"
#include "internal/default.h"
#include "internal/repl/shell/command/command.h"
#include "internal/repl/shell/command/command_internal.h"
#include "internal/repl/shell/token/token.h"
#include "command_internal.h"
#include "ft_assert.h"

static const char	*g_err_template = "minishell: syntax error "\
							"near unexpected token `%s'";

static int	find_paren_end(t_token **tokens, int tokens_len)
{
	int	open_paren;
	int	i;

	ft_assert(tokens[0]->type == OP_PAREN_OPEN, "expected paren open");
	i = 1;
	open_paren = 1;
	while (i < tokens_len)
	{
		if (tokens[i]->type == OP_PAREN_OPEN)
			open_paren++;
		else if (tokens[i]->type == OP_PAREN_CLOSE)
			open_paren--;
		if (open_paren == 0)
			break ;
		i++;
	}
	return (i);
}

int	is_valid_end(t_token *token)
{
	return (token
		&& (token->type == OP_EOF
			|| token->type == OP_NEWLINE
			|| token->type == OP_PIPE
			|| token->type == OP_AND
			|| token->type == OP_OR
			|| token->type == OP_PAREN_CLOSE));
}

t_command	command_build_parentheses(
	t_token **tokens, int op_idx, int tokens_len)
{
	t_command	cmd;
	int			paren_close_idx;

	(void) op_idx;
	ft_assert(tokens[0]->type == OP_PAREN_OPEN,
		"expected opening paren at command_cuild_parentheses");
	paren_close_idx = find_paren_end(tokens, tokens_len);
	if (tokens[paren_close_idx]->type != OP_PAREN_CLOSE \
		|| !is_valid_end(tokens[paren_close_idx + 1]) \
		|| paren_close_idx < 2)
	{
		paren_close_idx += (!is_valid_end(tokens[paren_close_idx + 1]) \
								&& paren_close_idx > 1);
		return (command_build_panic_zero(tokens[paren_close_idx]->content));
	}
	cmd = command_build(tokens + 1, paren_close_idx - 1);
	if (!cmd || cmd->type == CMD_INVALID)
		return (cmd);
	else if (command_build_is_empty(cmd))
		return (command_build_panic_one(cmd, \
			tokens[paren_close_idx]->content));
	return (command_paren_new(cmd));
}

t_command	command_build_paren_err(
	t_token **tokens, int cmd_operator_idx, int tokens_len)
{
	t_command	cmd;
	char		*err_msg;

	(void) tokens_len;
	ft_asprintf(&err_msg, g_err_template, tokens[cmd_operator_idx]->content);
	cmd = command_invalid_new(err_msg, EXIT_SYNTAX_ERROR);
	free(err_msg);
	return (cmd);
}
