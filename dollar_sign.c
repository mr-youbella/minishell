/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_sign.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 22:44:16 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/21 19:34:51 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	len_str(char *str, int status, t_list *environment, t_list **leaks)
{
	size_t	i;
	size_t	len_var;
	size_t	start;
	size_t	len;
	t_list	*new_leak;
	char	*env_value;
	char	*env_var;
	char	quote;

	i = 0;
	len = 0;
	quote = 0;
	while (str[i])
	{
		if (str[i] == 39 || str[i] == '"')
		{
			if (!quote)
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
				ft_lstadd_back(leaks, new_leak);
				len += ft_strlen(env_value);
				i += 2;
				continue ;
			}
			else if (!ft_isalpha(str[i + 1]) && str[i + 1] != '_')
			{
				i++;
				len++;
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
				ft_lstadd_back(leaks, new_leak);
				env_value = ft_getenv(env_var, environment, leaks);
				if (env_value)
					len += ft_strlen(env_value);
				continue ;
			}
		}
		i++;
		len++;
	}
	return (len);
}

char	*ft_dollar(char *str, t_list *environment, t_list **leaks)
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
	len = len_str(str, status, environment, leaks);
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
				ft_lstadd_back(leaks, new_leak);
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
				ft_lstadd_back(leaks, new_leak);
				env_value = ft_getenv(env_var, environment, leaks);
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
