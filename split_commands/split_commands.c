/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_commands.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 02:58:15 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 02:59:45 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static size_t	count_words(char **tokens)
{
	size_t	i;
	size_t	count;
	size_t	j;
	short	is_single_quote;
	short	is_double_quote;

	1 && (i = 0, count = 0);
	while (tokens[i])
	{
		1 && (j = 0, is_single_quote = 0, is_double_quote = 0);
		while (tokens[i][j])
		{
			if (tokens[i][j] == 39 && !is_double_quote)
				is_single_quote = !is_single_quote;
			else if (tokens[i][j] == '"' && !is_single_quote)
				is_double_quote = !is_double_quote;
			else if ((tokens[i][j] == ' ' || tokens[i][j] == '\t')
				&& !is_single_quote && !is_double_quote)
				count++;
			j++;
		}
		count++;
		i++;
	}
	return (count);
}

static short	check_tokens(size_t *k, size_t *i, size_t *j,
									t_ft_var *split_var)
{
	if (split_var->tokens[*i][*j] == 39 && !split_var->is_d_quote)
		return (split_var->is_s_quote = !split_var->is_s_quote, (*j)++, 0);
	else if (split_var->tokens[*i][*j] == '"' && !split_var->is_s_quote)
		return (split_var->is_d_quote = !split_var->is_d_quote, (*j)++, 0);
	else if ((split_var->tokens[*i][*j] == ' '
		|| split_var->tokens[*i][*j] == '\t')
		&& !split_var->is_s_quote && !split_var->is_d_quote)
	{
		if (split_var->buffer)
		{
			split_var->new_tokens[(*k)++] = split_var->buffer;
			split_var->buffer = NULL;
		}
		while ((split_var->tokens[*i][*j] == ' '
			|| split_var->tokens[*i][*j] == '\t'))
			(*j)++;
		return (0);
	}
	return (1);
}

static void	loop_splitcmd(size_t k, t_ft_var *split_var)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (split_var->tokens[i])
	{
		1 && (j = 0, split_var->is_s_quote = 0, split_var->is_d_quote = 0);
		split_var->buffer = NULL;
		while (split_var->tokens[i][j])
		{
			if (!check_tokens(&k, &i, &j, split_var))
				continue ;
			split_var->buffer = extract_word(split_var->buffer,
					split_var->tokens[i][j]);
			j++;
		}
		if (split_var->buffer)
			split_var->new_tokens[k++] = split_var->buffer;
		else
			split_var->new_tokens[k++] = ft_strdup("");
		i++;
	}
	split_var->new_tokens[k] = NULL;
}

char	**split_command(char *cmd_line, short is_dollar, t_var *variables)
{
	t_ft_var	*split_var;
	size_t		i;
	char		**tokens;
	char		**new_tokens;

	split_var = malloc(sizeof(t_ft_var));
	if (!split_var)
		return (NULL);
	i = 0;
	tokens = split_commmand_with_quotes(cmd_line, ' ', is_dollar, variables);
	if (!tokens)
		return (NULL);
	new_tokens = ft_calloc(count_words(tokens) + 1, sizeof(char *));
	if (!new_tokens)
		return (NULL);
	split_var->tokens = tokens;
	split_var->new_tokens = new_tokens;
	loop_splitcmd(0, split_var);
	i = 0;
	while (tokens[i])
		free(tokens[i++]);
	free(tokens);
	return (new_tokens);
}
