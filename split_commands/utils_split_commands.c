/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_split_commands.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 03:01:42 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 03:56:26 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

short	is_unclose_quotes(size_t single_quote, size_t double_quotes)
{
	if (single_quote % 2 || double_quotes % 2)
	{
		ft_putstr_fd("\033[34mminishell: \033[31msyntax error:", 2);
		ft_putstr_fd("\033[0m unclosed quote.\n", 2);
		ft_status(258, 1);
		return (1);
	}
	return (0);
}

char	*extract_word(char *str, char c)
{
	size_t	len_str;
	char	*p;

	len_str = 0;
	if (str)
		len_str = ft_strlen(str);
	p = ft_calloc(len_str + 2, 1);
	if (!p)
		return (NULL);
	if (str)
		ft_strlcpy(p, str, len_str + 2);
	p[len_str] = c;
	free(str);
	return (p);
}

static short	check_quote(char c, t_ft_var *split_var)
{
	if (c == 39 && !split_var->is_d_quote)
	{
		split_var->is_s_quote = !split_var->is_s_quote;
		split_var->in_token = 1;
		split_var->j++;
		return (0);
	}
	if (c == '"' && !split_var->is_s_quote)
	{
		split_var->is_d_quote = !split_var->is_d_quote;
		split_var->in_token = 1;
		split_var->j++;
		return (0);
	}
	return (1);
}

static short	add_count_with_redirection(char *cmd_line, char c,
											size_t *count, t_ft_var *split_var)
{
	if (!split_var->is_s_quote && !split_var->is_d_quote)
	{
		if (c == ' ')
		{
			if (split_var->in_token)
				1 && ((*count)++, split_var->in_token = 0);
			return (split_var->j++, 0);
		}
		if (c == '>' || c == '<')
		{
			if (split_var->in_token)
			{
				(*count)++;
				split_var->in_token = 0;
			}
			if ((c == '>' && cmd_line[split_var->j + 1] == '>')
				|| (c == '<' && cmd_line[split_var->j + 1] == '<'))
				split_var->j += 2;
			else
				split_var->j++;
			return ((*count)++, 0);
		}
	}
	return (1);
}

size_t	count_tokens_with_redirection(char *cmd_line)
{
	size_t		count;
	char		c;
	t_ft_var	*split_var;

	split_var = malloc(sizeof(t_ft_var));
	if (!split_var)
		return (0);
	1 && (split_var->j = 0, count = 0, split_var->is_s_quote = 0);
	1 && (split_var->is_s_quote = 0, split_var->is_d_quote = 0);
	split_var->in_token = 0;
	while (cmd_line[split_var->j])
	{
		c = cmd_line[split_var->j];
		if (!check_quote(c, split_var))
			continue ;
		if (!add_count_with_redirection(cmd_line, c, &count, split_var))
			continue ;
		split_var->in_token = 1;
		split_var->j++;
	}
	if (split_var->in_token)
		count++;
	return (count);
}
