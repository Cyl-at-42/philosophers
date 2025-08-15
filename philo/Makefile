# * Philosophers Makefile *

# Project name
NAME = philo

# Compiling flags
CFLAGS = -Wall -Wextra -Werror

# Source files
SRCS = philo.c philo_thread.c philo_utils.c simul.c simul_init.c

# Object files
OBJS = $(SRCS:.c=.o)

# Colors
RED	= "\033[0;31m"
GREEN = "\033[0;32m"
NO_COLOR = "\033[0m"

# Rules
all:$(NAME)

$(NAME):$(OBJS)
	@echo $(GREEN)Making $(NAME)$(NO_COLOR)
	cc $(CFLAGS) -o $(NAME) $(OBJS)

%.o:%.c
	@echo $(GREEN)Making object files$(NO_COLOR)
	cc $(CFLAGS) -o $@ -c $<

clean:
	@echo $(RED)Removing .o files$(NO_COLOR)
	rm -rf ${OBJS}

fclean: clean
	@echo $(RED)Removing $(NAME) $(NO_COLOR)
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus