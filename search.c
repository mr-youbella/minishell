/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 21:18:19 by wkannouf          #+#    #+#             */
/*   Updated: 2025/06/19 22:46:08 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_list	*search_in_list(char *str, t_list *list)
{
    while (list)
    {
        if (ft_strlen(str) == ft_strlen((char *)list->content) && !ft_strncmp(str, (char *)list->content, ft_strlen(str)))
            return (list);
        list = list->next;
    }
    return (NULL);
}
