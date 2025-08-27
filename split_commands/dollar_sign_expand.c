/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_sign_expand.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 22:44:16 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/27 12:56:10 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static size_t	len_str(char *str, t_var *variables)
{
	size_t		i;
	char		quote;
	t_dollar	*dollar_var;

	dollar_var = malloc(sizeof(t_dollar));
	if (!dollar_var)
		return (ft_status(1, 1), 0);
	ft_memset(dollar_var, 0, sizeof(t_dollar));
	1 && (i = 0, quote = 0);
	1 && (dollar_var->len = 0, dollar_var->str = str);
	while (str[i])
	{
		if (str[i] == 39 || str[i] == '"')
		{
			if (!quote)
				quote = str[i];
			else if (quote == str[i])
				quote = 0;
		}
		if (!len_dollar(&i, quote, variables, dollar_var))
			continue ;
		1 && (i++, dollar_var->len++);
	}
	i = dollar_var->len;
	return (free(dollar_var), i);
}

static void	dollar_expand(char *result, t_dollar *dollar_var, t_var *variables)
{
	char	*env_value;
	t_list	*new_leak;
	size_t	k;

	env_value = ft_itoa(ft_status(0, 0));
	new_leak = ft_lstnew(env_value);
	ft_lstadd_back(&variables->leaks, new_leak);
	k = 0;
	while (env_value[k])
		result[dollar_var->j++] = env_value[k++];
	dollar_var->i += 2;
}

static void	change_val(char *result, t_dollar *dollar_var,
					t_var *variables, size_t k)
{
	size_t	len_var;
	size_t	start;
	char	*env_var;
	t_list	*new_leak;
	char	*env_value;

	dollar_var->i++;
	start = dollar_var->i;
	len_var = 0;
	while (dollar_var->str[dollar_var->i]
		&& (ft_isalnum(dollar_var->str[dollar_var->i])
			|| dollar_var->str[dollar_var->i] == '_'))
		1 && (len_var++, dollar_var->i++);
	env_var = ft_substr(dollar_var->str, start, len_var);
	new_leak = ft_lstnew(env_var);
	ft_lstadd_back(&variables->leaks, new_leak);
	env_value = ft_getenv(env_var, variables);
	if (env_value)
	{
		k = 0;
		while (env_value[k])
			result[dollar_var->j++] = env_value[k++];
	}
}

static short	dollar_sign(t_dollar *dollar_var, t_var *variables,
						char *result)
{
	if (dollar_var->str[dollar_var->i] == 39
		|| dollar_var->str[dollar_var->i] == '"')
	{
		if (dollar_var->quote == 0)
			dollar_var->quote = dollar_var->str[dollar_var->i];
		else if (dollar_var->quote == dollar_var->str[dollar_var->i])
			dollar_var->quote = 0;
	}
	if (dollar_var->str[dollar_var->i] == '$')
	{
		if (dollar_var->str[dollar_var->i + 1] == '?'
			&& dollar_var->quote != 39)
			return (dollar_expand(result, dollar_var, variables), 0);
		else if (!ft_isalpha(dollar_var->str[dollar_var->i + 1])
			&& dollar_var->str[dollar_var->i + 1] != '_')
		{
			result[dollar_var->j++] = dollar_var->str[dollar_var->i++];
			return (0);
		}
		else if (dollar_var->quote != 39)
			return (change_val(result, dollar_var, variables, 0), 0);
	}
	return (1);
}

char	*ft_dollar(char *str, t_var *variables, size_t len)
{
	char		*result;
	t_dollar	*dollar_var;

	dollar_var = malloc(sizeof(t_dollar));
	if (!dollar_var)
		return (ft_status(1, 1), NULL);
	ft_memset(dollar_var, 0, sizeof(t_dollar));
	1 && (dollar_var->i = 0, dollar_var->j = 0);
	1 && (dollar_var->quote = 0, dollar_var->str = str);
	1 && (len = len_str(str, variables), result = malloc(len + 1));
	if (!result)
		return (free(dollar_var), ft_status(1, 1), NULL);
	while (str[dollar_var->i])
	{
		if (!dollar_sign(dollar_var, variables, result))
			continue ;
		result[dollar_var->j++] = str[dollar_var->i++];
	}
	return (result[dollar_var->j] = 0, free(dollar_var), result);
}
