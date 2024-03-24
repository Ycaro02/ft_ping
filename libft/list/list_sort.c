# include "linked_list.h" 

/* list sort function */
void    list_sort(t_list **lst, int (*cmp)())
{
	t_list  *next;
	t_list  *current;

	if (cmp == NULL || lst == NULL || *lst == NULL)
		return ;
	current = *lst;
	next = current;
	while (next && current && current->next)
	{
		next = current->next;
		if (cmp(next->content, current->content) <= 0) { /* if next value lower than current value reverse for strcmp works */

			void *tmp_data = current->content; /* just swap data using tmp */
			current->content = next->content;
			next->content = tmp_data;
			current = *lst;         /* reset current and next */
			continue ;
		}
		current = next;
	}
}