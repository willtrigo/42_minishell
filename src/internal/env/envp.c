/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dande-je <dande-je@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/17 23:12:04 by dande-je          #+#    #+#             */
/*   Updated: 2024/10/18 02:54:17 by dande-je         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_memlib.h"
#include "ft_stdio.h"
#include "ft_string.h"
#include "internal/default.h"
#include "internal/env/envp.h"
#include "internal/env/env_internal/env_internal.h"

static char	**build_envp(t_env_var *env_vars, char *env_var, int i, \
				t_envp_type type);

char	**get_envp(t_envp_type	type)
{
	t_env_var	*env_vars;
	char		*env_var;
	int			i;

	env_var = NULL;
	env_vars = env()->env_var;
	if (env_vars)
	{
		i = DEFAULT_INIT;
		return (build_envp(env_vars, env_var, i, type));
	}
	else
		return (NULL);
}

static char	**build_envp(t_env_var *env_vars, char *env_var, int i, \
				t_envp_type type)

{
	char	**envp;

	envp = ft_calloc(env()->env_size + NULL_BYTE, sizeof(char *));
	while (env_vars)
	{
		if (type == ENVP_EXPORT && \
			!(ft_strncmp(env_vars->key, "_=", ft_strlen("_=")) == DEFAULT))
		{
			if (env_vars->key[ft_strlen(env_vars->key) - CHAR_BYTE] != '=')
				ft_asprintf(&env_var, "%s", env_vars->key);
			else if (env_vars->key[ft_strlen(env_vars->key) - CHAR_BYTE] == '=')
				ft_asprintf(&env_var, "%s\"%s\"", env_vars->key, \
					env_vars->value);
			envp[++i] = env_var;
		}
		else if (type == ENVP_DEFAULT && env_vars->key[ft_strlen(env_vars->key) - CHAR_BYTE] == '=')
		{
			ft_asprintf(&env_var, "%s%s", env_vars->key, env_vars->value);
			envp[++i] = env_var;
		}
		env_vars = env_vars->next;
	}
	return (envp);
}
