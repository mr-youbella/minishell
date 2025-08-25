/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_sign.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 22:44:16 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/25 14:00:01 by youbella         ###   ########.fr       */
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
			env_value = ft_itoa(dollar_var->status);
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

static size_t	len_str(char *str, int status, t_var *variables)
{
	size_t		i;
	char		quote;
	t_dollar	*dollar_var;

	dollar_var = malloc(sizeof(t_dollar));
	if (!dollar_var)
		return (0);
	1 && (i = 0, quote = 0);
	1 && (dollar_var->len = 0, dollar_var->str = str);
	dollar_var->status = status;
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

char	*ft_dollar(char *str, t_var *variables)
{
	char	*env_value;
	char	*env_var;
	char	*result;
	size_t	len;
	size_t	i;
	size_t	j;
	size_t	k;
	size_t	start;
	size_t	len_var;
	t_list	*new_leak;
	char	quote;
	int		status;

	i = 0;
	j = 0;
	quote = 0;
	status = ft_status(0, 0);
	len = len_str(str, status, variables);
	result = malloc(len + 1);
	if (!result)
		return (NULL);
	while (str[i])
	{
		if (str[i] == 39 || str[i] == '"')
		{
			if (quote == 0)
				quote = str[i];
			else if (quote == str[i])
				quote = 0;
		}
		if (str[i] == '$')
		{
			if (str[i + 1] == '?' && quote != 39)
			{
				env_value = ft_itoa(status);
				new_leak = ft_lstnew(env_value);
				ft_lstadd_back(&variables->leaks, new_leak);
				k = 0;
				while (env_value[k])
					result[j++] = env_value[k++];
				i += 2;
				continue ;
			}
			else if (!ft_isalpha(str[i + 1]) && str[i + 1] != '_')
			{
				result[j++] = str[i++];
				continue ;
			}
			else if (quote != 39)
			{
				i++;
				start = i;
				len_var = 0;
				while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
				{
					len_var++;
					i++;
				}
				env_var = ft_substr(str, start, len_var);
				new_leak = ft_lstnew(env_var);
				ft_lstadd_back(&variables->leaks, new_leak);
				env_value = ft_getenv(env_var, variables);
				if (env_value)
				{
					k = 0;
					while (env_value[k])
						result[j++] = env_value[k++];
				}
				continue ;
			}
		}
		result[j++] = str[i++];
	}
	result[j] = 0;
	return (result);
}
