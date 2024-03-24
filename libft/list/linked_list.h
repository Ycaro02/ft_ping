#ifndef DEFINE_LINKED_LIST_H
# define DEFINE_LINKED_LIST_H

# include "../libft.h"
# include "../basic_define.h"

 /********************************************************************
 *                        DATA STRUCTURES                           *
 ********************************************************************/

/**
 * Simple linked list
 * void content need to be casted to acces value
*/
typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}	t_list;

 /********************************************************************
 *                        PUBLIC FUNCTIONS                          *
 ********************************************************************/
t_list			*ft_lstnew(void *content);
void			ft_lstadd_front(t_list **lst, t_list *new);
int				ft_lstsize(t_list *lst);
t_list			*ft_lstlast(t_list *lst);
void			ft_lstadd_back(t_list **lst, t_list *new);
void			ft_lstdelone(t_list *lst, void (*del)(void *));
void			ft_lstclear(t_list **lst, void (*del)(void*));
void			ft_lstiter(t_list *lst, void (*f)(void *));
t_list			*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *));
t_list 			*ft_lstjoin(t_list *first, t_list *second);
void 			ft_lstclear_nodeptr(t_list **lst);
void			ft_lst_remove_if(t_list **list, void *ref, void (*del)(), int (*condition)());
void 			*get_lst_index_content(t_list *lst, int index);

void			lst_clear(t_list **lst, void (*del)(void*));
void 			reverse_lst(t_list **lst);

void			list_sort(t_list **lst, int (*cmp)());




 # endif /* DEFINE_LINKED_LIST_H */