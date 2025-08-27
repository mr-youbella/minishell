/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_cmd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 17:29:22 by youbella          #+#    #+#             */
/*   Updated: 2025/08/27 17:29:41 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	print_errno(char **tokens)
{
	ft_putstr_fd("\033[34mminishell: \033[31m", 2);
	ft_putstr_fd(tokens[0], 2);
	ft_putstr_fd("\033[0m: ", 2);
	ft_putstr_fd(strerror(errno), 2);
	ft_putstr_fd(".\n", 2);
}
