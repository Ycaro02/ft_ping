OBJ_DIR			=	obj

SRC_DIR 		=	src

MAIN_MANDATORY 	=	main.c

SRCS			=	handle_socket.c\
					listen_icmp_reply.c\
					build_request.c\
					checksum.c\
					random_data.c\
					handle_time.c\
					network_utils.c\
					send_ping.c\
					detail_display.c\
					ping_context.c\
					handle_summary.c\


MAIN_BONUS		=	main_bonus.c\

MAKE_LIBFT		=	make -s -C libft

MAKE_LIST		=	make -s -C libft/list

LIBFT			= 	libft/libft.a

LIST			= 	libft/list/linked_list.a

OBJS 			= $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

RM			=	rm -rf

VERSION			= mandatory

ifeq ($(findstring bonus, $(MAKECMDGOALS)), bonus)
ASCII_NAME	= "bonus"
SRCS += $(MAIN_BONUS)
VERSION = bonus
else
SRCS += $(MAIN_MANDATORY)
endif