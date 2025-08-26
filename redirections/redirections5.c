/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections5.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 03:18:01 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 23:51:18 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

short	is_exist_redirect_pipe(char *cmd_line, char redirect_pipe)
{
	size_t	i;
	char	quot;

	i = 0;
	while (cmd_line[i])
	{
		if (cmd_line[i] == '"' || cmd_line[i] == 39)
		{
			quot = cmd_line[i];
			i++;
			while (cmd_line[i] && cmd_line[i] != quot)
				i++;
			if (cmd_line[i] == quot)
				i++;
		}
		if (!cmd_line[i])
			break ;
		if ((redirect_pipe == 'r' && cmd_line[i] == '>')
			|| (redirect_pipe == 'r' && cmd_line[i] == '<')
			|| (redirect_pipe == '|' && cmd_line[i] == '|'))
			return (1);
		i++;
	}
	return (0);
}

static short	check_ambiguous_dollar(char *str, t_var *variables,
										t_ft_var *var)
{
	size_t	start;
	size_t	var_len;
	size_t	j;
	char	*var_name;
	char	*var_value;

	var_value = NULL;
	if (str[var->j] == '$')
	{
		1 && (j = 0, var_len = 0, start = var->j, var->j++);
		while ((str[var->j]) && (ft_isalnum(str[var->j]) || str[var->j] == '_'))
			1 && (var_len++, var->j++);
		var_name = ft_substr(str, start, var_len + 1);
		var_value = ft_dollar(var_name, variables, 0);
		free(var_name);
		while (var_value[j])
		{
			if (var_value[j] == ' ')
				return (free(var_value), 1);
			j++;
		}
	}
	else
		var->j++;
	return (free(var_value), 0);
}

short	is_ambiguous_redirect(char *token, t_var *variables)
{
	t_ft_var	*var;
	char		quot;

	var = malloc(sizeof(t_ft_var));
	if (!var)
		return (ft_status(1, 1), 0);
	ft_memset(var, 0, sizeof(t_ft_var));
	var->j = 0;
	while (token[var->j])
	{
		if (token[var->j] == 39 || token[var->j] == '"')
		{
			quot = token[var->j];
			var->j++;
			while (token[var->j] && token[var->j] != quot)
				var->j++;
			if (token[var->j] == quot)
				var->j++;
		}
		if (!token[var->j])
			break ;
		if (check_ambiguous_dollar(token, variables, var))
			return (free(var), 1);
	}
	return (free(var), 0);
}

short	check_syntax_redirect(char **tokens, t_ft_var *list_var)
{
	if ((ft_strlen(tokens[list_var->j]) == 2
			&& ((!ft_strncmp(tokens[list_var->j], ">>", 2))
				|| (!ft_strncmp(tokens[list_var->j], "<<", 2))))
		|| ((ft_strlen(tokens[list_var->j]) == 1)
			&& ((!ft_strncmp(tokens[list_var->j], ">", 1))
				|| (!ft_strncmp(tokens[list_var->j], "<", 1)))))
	{
		list_var->j++;
		if ((!tokens[list_var->j] || (ft_strlen(tokens[list_var->j]) == 2
					&& !ft_strncmp(tokens[list_var->j], ">>", 2))
				|| !ft_strncmp(tokens[list_var->j], "<<", 2))
			|| (!tokens[list_var->j] || (ft_strlen(tokens[list_var->j]) == 1
					&& (!ft_strncmp(tokens[list_var->j], ">", 1)
						|| !ft_strncmp(tokens[list_var->j], "<", 1)))))
		{
			ft_putstr_fd("\033[34mminishell:\033[31m ", 2);
			ft_putstr_fd("syntax error in redirection.\n\033[0m", 2);
			return (ft_status(258, 1), -1);
		}
	}
	else
		return (list_var->j++, 0);
	return (1);
}
