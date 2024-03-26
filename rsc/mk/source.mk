OBJ_DIR			=	obj

SRC_DIR 		=	src

SRCS			=	main.c\
					handle_socket.c\
					listen_icmp_reply.c\
					build_request.c\
					checksum.c\
					random_data.c\


MAKE_LIBFT		=	make -s -C libft

MAKE_LIST		=	make -s -C libft/list

LIBFT			= 	libft/libft.a

LIST			= 	libft/list/linked_list.a

OBJS 			= $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

RM			=	rm -rf
