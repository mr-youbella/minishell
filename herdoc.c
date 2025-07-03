#include "minishell.h"

typedef struct s_redirect
{
	char	*type_redirection;
	char	*file_name;
	struct s_redirect	*next;
}	t_redirect;

t_redirect	*lstlast(t_redirect *lst)
{
	if (!lst)
		return (NULL);
	while (lst->next)
		lst = lst->next;
	return (lst);
}

void	lstadd_back(t_redirect **lst, t_redirect *new)
{
	if (!new)
		return ;
	if (!*lst)
	{
		*lst = new;
		return ;
	}
	lstlast(*lst)->next = new;
}


t_redirect	*lstnew(char *file, char *redirection)
{
	t_redirect	*node;

	node = malloc(sizeof(t_redirect));
	if (!node)
		return (NULL);
	node->type_redirection = redirection;
	node->file_name = file;
	node->next = NULL;
	return (node);
}

t_redirect	*search_herdoc_end_name_file(char *str)
{
	size_t		i;
	size_t		len;
	size_t		start;
	t_redirect	*red;
	t_redirect	*new_node;
	char		*re;
	char		*end_file;

	start = 0;
	len = 0;
	i = 0;
	red = NULL;
	while(str[i])
	{
		if (str[i] == '<')
		{
			if (str[i + 1] == '<')
			{
				i += 2;
				if (str[i] == 0)
				{
					printf("minishell: Syntax error.\n");
					return (NULL);
				}
				re = ft_strdup("<<");
			}
			while (str[i] == ' ')
				i++;
			start = i;
			while (str[i] && str[i] != '<' && str[i] != ' ')
				i++;
			len = i - start;
			end_file = ft_substr(str, start, len);
			new_node = lstnew(end_file, re);
			lstadd_back(&red, new_node);
		}
		else 
			i++;
	}
	return (red);
}

short    is_there_herdoc_out(const char *cmd_line)
{
	size_t	i;

	i = 0;
	while(cmd_line[i])
	{
		if (cmd_line[i] == '<' && cmd_line[i + 1] == '<')
			return (1);
		i++;
	}
	return (0);
}

size_t    strcpy_until_herdoc(char *dst, const char *src, size_t n)
{
	size_t	i;

	i = 0;
	if (!n)
		return (ft_strlen(src));
	while (src[i] && i < n - 1 && src[i] != '<' && src[i + 1] != '<')
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = 0;
	printf ("%s\n", dst);
	return (ft_strlen(src));
}

size_t	len_str_herdoc(char *str)
{
	size_t	i;

	if (!str)
		return (0);
	while (str[i] && str[i] != '<' && str[i + 1] != '<')
		i++;
	return (i);
}
