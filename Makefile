# compiler
CC			= gcc

# flags
CFLAGS		= -g -Wall -Wextra -Werror -pthread

# artifacts
NAME		= philosophers

# source files
SRCS	= cleaning.c \
		death_checker.c \
		initialization.c \
		main.c \
		parsing.c \
		routine_functions.c \
		routine_functions2.c \
		routine.c \
		utils.c 

# object files
OBJS	= ${SRCS:.c=.o}

# so long library
PHILO		= philo.h

# rules
all: ${NAME}

${NAME}		: ${OBJS} philo.h
			${CC} ${CFLAGS} ${OBJS} ${PHILO} -o $(NAME) 

%.o 		: %.c
			${CC} ${CFLAGS} -c $< -o $@ 

clean		:
			rm -rf ${OBJS}

fclean		: clean
			rm -rf ${NAME}

re			: fclean all

val			:${NAME}
			valgrind \
			--leak-check=full --tool=memcheck \
			--show-reachable=yes \
			--track-fds=yes \
			--errors-for-leak-kinds=all \
			--show-leak-kinds=all ./${NAME}

norme		:
			norminette ${SRCS} ${PHILO}

.PHONY: all clean fclean re