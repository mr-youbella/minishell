/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 06:03:27 by wkannouf          #+#    #+#             */
/*   Updated: 2025/06/22 22:06:12 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <stdio.h>
# include <signal.h>
# include <termios.h>
# include <fcntl.h>
# include <readline/readline.h>
# include <readline/history.h>

# define GREEN "\033[32m"
# define RED "\033[31m"
# define BGRED "\033[41m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define CYAN "\033[0;36m"
# define PINK "\033[35m"
# define DEF "\033[0m"

typedef struct s_redirections_output
{
	char				*type_redirection;
	char				*file_name;
	struct s_redirections_output	*next;
}	t_redirections_output;

char					**ft_split_first_cmd(char const *s, char c, int status);
t_list					*search_in_list(char *str, t_list *list);
short					check_export_arg(char *arg);
short					check_unset_arg(char *arg);
char					*get_next_line(int fd);
short					is_there_redirect_out(char *cmd_line);
char					*join_tokens(char **tokens);
size_t					strcpy_until_redirections(char *dst, const char *src, size_t n);
t_redirections_output	*add_redirections_out_and_filename_in_list(char *str);

#endif
