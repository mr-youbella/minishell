/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_sign_expand2.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 03:58:57 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 04:01:25 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	len_dollar2(size_t	*i, t_var *variables, t_dollar *dollar_var)
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
