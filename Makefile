include rsc/mk/color.mk
include rsc/mk/source.mk

NAME			=	ft_ping
CC				=	gcc
CFLAGS			=	-Wall -Wextra -Werror -O3 -g 
ASCII_ART		=	./rsc/mk/ascii.sh
ASCII_NAME		=	${NAME}

PING_ADDR		=	192.168.1.1

all:		$(NAME)

%.o : %.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME):	$(OBJ_DIR) $(OBJS) $(DISPLAY_NAME) $(LIST) $(LIBFT)
	@printf "$(CYAN)Compiling ${NAME} ...$(RESET)\n"
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBFT) $(LIST) -lm
	@printf "$(GREEN)Compiling $(NAME) done$(RESET)\n"

$(LIST):
ifeq ($(shell [ -f ${LIST} ] && echo 0 || echo 1), 1)
	@printf "$(CYAN)Compiling list...$(RESET)\n"
	@$(MAKE_LIST)
	@printf "$(GREEN)Compiling list done$(RESET)\n"
endif

$(LIBFT):
ifeq ($(shell [ -f ${LIBFT} ] && echo 0 || echo 1), 1)
	@printf "$(CYAN)Compiling libft...$(RESET)\n"
	@$(MAKE_LIBFT)
	@printf "$(GREEN)Compiling libft done$(RESET)\n"
endif

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)
	@$(ASCII_ART) $(ASCII_NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@printf "$(YELLOW)Compile $<$(RESET)\n"
	@$(CC) -o $@ -c $< $(CFLAGS)

clean:
ifeq ($(shell [ -d ${OBJ_DIR} ] && echo 0 || echo 1), 0)
	@$(RM) $(OBJ_DIR)
	@printf "$(RED)Clean $(OBJ_DIR)/test output$(RESET)\n"
	@$(RM) ${TESTER_OUT_FILES}
endif

fclean:		clean
	@printf "$(RED)Clean $(NAME)/lib$(RESET)\n"
	@$(RM) $(NAME) ${TESTER_OUT_FILES} ${DISPLAY_NAME}

clean_lib:
	@$(MAKE_LIBFT) fclean
	@$(MAKE_LIST) fclean

test: $(NAME)
	@printf "$(CYAN)Test $(NAME) ${PING_ADDR} $(RESET)\n"
	@sudo ./$(NAME) $(PING_ADDR)

vtest: $(NAME)
	@sudo valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./$(NAME) $(PING_ADDR)

re:			fclean all

.PHONY:		all clean fclean re bonus
