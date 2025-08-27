/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections4.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 15:23:33 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/27 23:01:02 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static short	add_redirections(char **tokens, t_ft_var *list_var,
							t_redirections **new_node, t_var *variables)
{
	char	**file_name;

	if (ft_strlen(tokens[list_var->j - 1]) == 2
		&& !ft_strncmp(tokens[list_var->j - 1], "<<", 2))
	{
		file_name = split_command(tokens[list_var->j], 0, variables);
		*new_node = craete_new_node(tokens[list_var->j - 1], file_name[0]);
		free(file_name);
		return (1);
	}
	else if (is_ambiguous_redirect(tokens[list_var->j], variables))
	{
		tokens[list_var->j] = ft_strdup("");
		*new_node = craete_new_node(tokens[list_var->j - 1],
				tokens[list_var->j]);
	}
	else
	{
		file_name = split_command(tokens[list_var->j], 1, variables);
		*new_node = craete_new_node(tokens[list_var->j - 1], file_name[0]);
		free(file_name);
	}
	return (0);
}

static short	loop_redirections(char **tokens, t_ft_var *list_var,
							t_var *variables, t_redirections **list)
{
	short			return_val;
	size_t			count_herdoc;
	t_redirections	*new_node;

	count_herdoc = 0;
	while (tokens[list_var->j])
	{
		return_val = check_syntax_redirect(tokens, list_var);
		if (!return_val)
			continue ;
		if (return_val == -1)
			return (free_list(variables, NULL, *list), free(list_var), 0);
		count_herdoc += add_redirections
			(tokens, list_var, &new_node, variables);
		if (count_herdoc >= 17)
		{
			ft_putstr_fd("\033[34minishell: ", 2);
			ft_putstr_fd
				("\033[31mmaximum here-document count exceeded.\033[0m\n", 2);
			free_list(variables, NULL, *list);
			return (ft_status(2, 1), free(list_var), 0);
		}
		add_node_in_back(list, new_node);
		list_var->j++;
	}
	return (1);
}

t_redirections	*list_redirections(char **tokens, t_var *variables)
{
	t_redirections	*list;
	t_ft_var		*list_var;

	list_var = malloc(sizeof(t_ft_var));
	if (!list_var)
		return (ft_status(1, 1), NULL);
	ft_memset(list_var, 0, sizeof(t_ft_var));
	1 && (list_var->j = 0, list = NULL);
	if (!loop_redirections(tokens, list_var, variables, &list))
		return (NULL);
	return (free(list_var), list);
}

static char	*get_command(char **tokens, size_t i, char *tmp, char *cmd_arg)
{
	while (tokens[i])
	{
		if ((ft_strlen(tokens[i]) == 1 && !ft_strncmp(tokens[i], ">", 1))
			|| (ft_strlen(tokens[i]) == 2 && !ft_strncmp(tokens[i], ">>", 2))
			|| (ft_strlen(tokens[i]) == 1 && !ft_strncmp(tokens[i], "<", 1))
			|| (ft_strlen(tokens[i]) == 2 && !ft_strncmp(tokens[i], "<<", 2)))
		{
			if (tokens[i + 1])
				i += 2;
			else
				i++;
		}
		else
		{
			tmp = cmd_arg;
			cmd_arg = ft_strjoin(cmd_arg, tokens[i]);
			free(tmp);
			tmp = cmd_arg;
			cmd_arg = ft_strjoin(cmd_arg, " ");
			free(tmp);
			i++;
		}
	}
	return (cmd_arg);
}

char	*join_cmd_args(char *cmd_line)
{
	size_t	i;
	char	**tokens;
	char	*cmd_arg;

	i = 0;
	tokens = get_tokens_with_redirection(cmd_line);
	if (!tokens)
		return (NULL);
	if ((ft_strlen(tokens[i]) == 1 && !ft_strncmp(tokens[i], ">", 1))
		|| (ft_strlen(tokens[i]) == 2 && !ft_strncmp(tokens[i], ">>", 2))
		|| (ft_strlen(tokens[i]) == 1 && !ft_strncmp(tokens[i], "<", 1))
		|| (ft_strlen(tokens[i]) == 2 && !ft_strncmp(tokens[i], "<<", 2)))
	{
		i++;
		if (tokens[i])
			i++;
	}
	cmd_arg = get_command(tokens, i, NULL, NULL);
	i = 0;
	while (tokens[i])
		free(tokens[i++]);
	free(tokens);
	return (cmd_arg);
}
