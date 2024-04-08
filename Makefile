include rsc/mk/color.mk
include rsc/mk/source.mk

NAME			=	ft_ping
CC				=	gcc
CFLAGS			=	-Wall -Wextra -Werror -O3 -g 
ASCII_ART		=	./rsc/mk/ascii.sh
ASCII_NAME		=	${NAME}

MANDATORY_ARGS	=	localhost --verbose -v 127.0.0.1
BONUS_ARGS		=	-w2 localhost -c1 --count 2 -W 5 --count1 --pattern ffffaaaa333388 --ttl4 --count3

RUN_TEST		=	./rsc/sh/run_test.sh
VALGRIND_TEST	=	./rsc/sh/valgrind_test.sh

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

bonus: clear_mandatory ${NAME}

clear_mandatory:
ifeq ($(shell [ -f ${OBJ_DIR}/main.o ] && echo 0 || echo 1), 0)
	@printf "$(RED)Clean mandatory obj $(RESET)\n"
	@rm -rf ${OBJ_DIR}
endif

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
	@printf "$(CYAN)Test $(NAME) ${MANDATORY_ARGS} $(RESET)\n"
	@sudo ./$(RUN_TEST) $(MANDATORY_ARGS)

vtest: $(NAME)
	@sudo ./$(VALGRIND_TEST) $(MANDATORY_ARGS)

btest:
	@make -s bonus
	@printf "$(CYAN)Test $(NAME) ${BONUS_ARGS} $(RESET)\n"
	@sudo ./$(RUN_TEST) $(BONUS_ARGS)

bvtest:
	@make -s bonus
	@printf "$(CYAN)Test $(NAME) ${BONUS_ARGS} $(RESET)\n"
	@sudo ./$(VALGRIND_TEST) $(BONUS_ARGS)

re:			fclean all

.PHONY:		all clean fclean re bonus
