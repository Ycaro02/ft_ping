# include "libft.h"
# include "vector.h"

/* @brief display vector list */
void display_lst_vec(t_list *lst)
{
    t_list *tmp = lst;
    while(tmp)
    {
        t_vec *point = tmp->content;
        ft_printf_fd(1, "%spos : [%d][%d]%s\n",RED,point->y,point->x,RESET);
        tmp = tmp->next;
    }
}

/* @brief create a vector */
t_vec create_vector(uint32_t y, uint32_t x) {
        t_vec vec;
        vec.x = x;
        vec.y = y;
        return (vec);
}

/* @brief add vector */
t_vec        add_vector(t_vec a, t_vec b) {
        a.x += b.x;
        a.y += b.y;
        return (a);
}

/* @brief sub vector */
t_vec        sub_vector(t_vec a, t_vec b) {
        a.x -= b.x;
        a.y -= b.y;
        return (a);
}

/* @brief mult vector */
t_vec        mult_vec(t_vec a, t_vec b) {
        a.x *= b.x;
        a.y *= b.y;
        return (a);
}

/* @brief div vector */
t_vec        div_vector(t_vec a, t_vec b) {
        a.x /= b.x;
        a.y /= b.y;
        return (a);
}

/**
 * @brief Compare two vector
 * @return 1 if equal, 0 if not
*/
int          vector_cmp(t_vec a, t_vec b) {
        if (a.y == b.y && a.x == b.x)
                return (1);
        return (0);
}


/**
 * @Get the absolute difference between vector and 2 int point
*/
// t_vec        vector_abs_diff(t_vec a, t_vec b) {
//     t_vec diff = { 
// 		.y = abs_diff(a.y, b.y),
// 		.x = abs_diff(a.x, b.x)
// 	};
//     return (diff);
// }