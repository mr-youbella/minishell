/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_sign.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 22:44:16 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/25 14:57:36 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	len_dollar2(size_t	*i, t_var *variables, t_dollar *dollar_var)
{
	size_t	start;
	size_t	len_var;
	char	*env_var;
	t_list	*new_leak;
	char	*env_value;

	(*i)++;
	start = *i;
	len_var = 0;
	while (dollar_var->str[*i] && (ft_isalnum(dollar_var->str[*i])
			|| dollar_var->str[*i] == '_'))
	{
		len_var++;
		(*i)++;
	}
	env_var = ft_substr(dollar_var->str, start, len_var);
	new_leak = ft_lstnew(env_var);
	ft_lstadd_back(&variables->leaks, new_leak);
	env_value = ft_getenv(env_var, variables);
	if (env_value)
		dollar_var->len += ft_strlen(env_value);
}

short	len_dollar(size_t *i, char quote,
					t_var *variables, t_dollar *dollar_var)
{
	char	*env_value;
	t_list	*new_leak;

	if (dollar_var->str[*i] == '$')
	{
		if (dollar_var->str[*i + 1] == '?' && quote != 39)
		{
			env_value = ft_itoa(ft_status(0, 0));
			new_leak = ft_lstnew(env_value);
			ft_lstadd_back(&variables->leaks, new_leak);
			dollar_var->len += ft_strlen(env_value);
			*i += 2;
			return (0);
		}
		else if (!ft_isalpha(dollar_var->str[*i + 1])
			&& dollar_var->str[*i + 1] != '_')
		{
			(*i)++;
			dollar_var->len++;
			return (0);
		}
		else if (quote != 39)
			return (len_dollar2(i, variables, dollar_var), 0);
	}
	return (1);
}

static size_t	len_str(char *str, t_var *variables)
{
	size_t		i;
	char		quote;
	t_dollar	*dollar_var;

	dollar_var = malloc(sizeof(t_dollar));
	if (!dollar_var)
		return (0);
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
		i++;
		dollar_var->len++;
	}
	return (dollar_var->len);
}

void	dollar_expand(char *result, t_dollar *dollar_var, t_var *variables)
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

void	change_val(char *result, t_dollar *dollar_var,
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

short	dollar_sign(t_dollar *dollar_var, t_var *variables,
						char quote, char *result)
{
	if (dollar_var->str[dollar_var->i] == '$')
	{
		if (dollar_var->str[dollar_var->i + 1] == '?' && quote != 39)
			return (dollar_expand(result, dollar_var, variables), 0);
		else if (!ft_isalpha(dollar_var->str[dollar_var->i + 1])
			&& dollar_var->str[dollar_var->i + 1] != '_')
		{
			result[dollar_var->j++] = dollar_var->str[dollar_var->i++];
			return (0);
		}
		else if (quote != 39)
		{
			change_val(result, dollar_var, variables, 0);
			return (0);
		}
	}
	return (1);
}

char	*ft_dollar(char *str, t_var *variables, size_t len, char quote)
{
	char		*result;
	t_dollar	*dollar_var;

	dollar_var = malloc(sizeof(t_dollar));
	if (!dollar_var)
		return (NULL);
	1 && (dollar_var->i = 0, dollar_var->j = 0);
	1 && (quote = 0, dollar_var->str = str);
	1 && (len = len_str(str, variables), result = malloc(len + 1));
	if (!result)
		return (NULL);
	while (str[dollar_var->i])
	{
		if (str[dollar_var->i] == 39 || str[dollar_var->i] == '"')
		{
			if (quote == 0)
				quote = str[dollar_var->i];
			else if (quote == str[dollar_var->i])
				quote = 0;
		}
		if (!dollar_sign(dollar_var, variables, quote, result))
			continue ;
		result[dollar_var->j++] = str[dollar_var->i++];
	}
	return (result[dollar_var->j] = 0, result);
}
