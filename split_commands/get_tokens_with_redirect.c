/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_tokens_with_redirect.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 03:07:57 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 03:27:43 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static short	get_redirections(char *cmd_line, char c,
								size_t *i, t_ft_var *sv)
{
	char	*op;

	if (!sv->is_s_quote && !sv->is_d_quote && (c == '>' || c == '<'))
	{
		if (sv->buffer)
		{
			1 && (sv->tokens[sv->j++] = sv->buffer, sv->buffer = NULL);
		}
		if (c == '>' && cmd_line[*i + 1] == '>')
		{
			sv->tokens[sv->j++] = ft_strdup(">>");
			return ((*i) += 2, 0);
		}
		else if (c == '<' && cmd_line[*i + 1] == '<')
		{
			sv->tokens[sv->j++] = ft_strdup("<<");
			return ((*i) += 2, 0);
		}
		else
		{
			1 && (op = extract_word(NULL, c), sv->tokens[sv->j++] = op);
			return ((*i)++, 0);
		}
	}
	return (1);
}

static void	loop_tokens_wr(char *cmd_line, t_ft_var *sv)
{
	size_t	i;
	char	c;

	1 && (sv->is_s_quote = 0, sv->is_d_quote = 0);
	i = 0;
	while (cmd_line[i])
	{
		c = cmd_line[i];
		if (c == 39 && !sv->is_d_quote)
			sv->is_s_quote = !sv->is_s_quote;
		else if (c == '"' && !sv->is_s_quote)
			sv->is_d_quote = !sv->is_d_quote;
		if (!get_redirections(cmd_line, c, &i, sv))
			continue ;
		if (!sv->is_s_quote && !sv->is_d_quote && c == ' ')
		{
			if (sv->buffer)
				1 && (sv->tokens[sv->j++] = sv->buffer, sv->buffer = NULL);
			i++;
			continue ;
		}
		sv->buffer = extract_word(sv->buffer, c);
		i++;
	}
}

char	**get_tokens_with_redirection(char *cmd_line)
{
	char		**tokens;
	t_ft_var	*split_var;

	split_var = malloc(sizeof(t_ft_var));
	if (!split_var)
		return (NULL);
	split_var->j = 0;
	split_var->buffer = NULL;
	tokens = malloc((count_tokens_with_redirection(cmd_line) + 1)
			* sizeof(char *));
	if (!tokens)
		return (NULL);
	split_var->tokens = tokens;
	loop_tokens_wr(cmd_line, split_var);
	if (split_var->buffer)
		tokens[split_var->j++] = split_var->buffer;
	tokens[split_var->j] = NULL;
	return (tokens);
}
