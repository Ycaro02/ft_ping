#ifndef DEFINE_VECTOR_HEADER
# define DEFINE_VECTOR_HEADER

# include <sys/types.h>

 /********************************************************************
 *                        DATA STRUCTURES                           *
 ********************************************************************/

/* t_vec 2d point vector */
typedef struct vec_t {
	uint32_t	x;
	uint32_t	y;
}	t_vec;

 /********************************************************************
 *                        PUBLIC FUNCTIONS                          *
 ********************************************************************/

t_vec		create_vector(uint32_t y, uint32_t x);
t_vec		add_vector(t_vec a, t_vec b);
t_vec		sub_vector(t_vec a, t_vec b);
t_vec		mult_vec(t_vec a, t_vec b);
t_vec		div_vector(t_vec a, t_vec b);
int			vector_cmp(t_vec a, t_vec b);
t_vec		vector_abs_diff(t_vec a, t_vec b);

 # endif /* DEFINE_VECTOR_HEADER */
