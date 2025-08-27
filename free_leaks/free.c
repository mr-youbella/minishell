/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 03:30:29 by youbella          #+#    #+#             */
/*   Updated: 2025/08/27 17:32:31 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_leaks(t_var *variables)
{
	t_list	*tmp;

	while (variables->leaks)
	{
		free(variables->leaks->content);
		tmp = variables->leaks;
		variables->leaks = variables->leaks->next;
		free(tmp);
	}
	while (variables->export_list)
	{
		tmp = variables->export_list;
		variables->export_list = variables->export_list->next;
		free(tmp);
	}
}

void	free_list(t_var *variables, t_list *list, t_redirections *redirections)
{
	t_list	*new_leak;

	while (redirections)
	{
		new_leak = ft_lstnew(redirections->file_name);
		ft_lstadd_back(&variables->leaks, new_leak);
		new_leak = ft_lstnew(redirections);
		ft_lstadd_back(&variables->leaks, new_leak);
		redirections = redirections->next;
	}
	while (list)
	{
		new_leak = ft_lstnew(list);
		ft_lstadd_back(&variables->leaks, new_leak);
		list = list->next;
	}
}

void	free_array(char **arr, short is_stock, t_var *variables)
{
	size_t	i;
	t_list	*new_leak;

	i = 0;
	if (is_stock)
	{
		while (arr && arr[i])
		{
			new_leak = ft_lstnew(arr[i]);
			ft_lstadd_back(&variables->leaks, new_leak);
			i++;
			if (!arr[i])
			{
				new_leak = ft_lstnew(arr);
				ft_lstadd_back(&variables->leaks, new_leak);
			}
		}
	}
	else
	{
		while (arr && arr[i])
			free(arr[i++]);
		free(arr);
	}
}

void	vr_free_close(t_var_redirect *var_redirection)
{
	if (var_redirection->fd_file_input > 0)
		close(var_redirection->fd_file_input);
	if (var_redirection->fd_file_output > 0)
		close(var_redirection->fd_file_output);
	free(var_redirection->pipe_output);
	free(var_redirection->join_herdoc);
	free(var_redirection->cmd_result);
}
