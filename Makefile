NAME = webserv

OBJS_DIR = obj/
SRCS_DIR = src/


SRCS = 	src/conf.cpp\
		src/dir_handler.cpp\
		src/parsing.cpp\
		src/utils.cpp\
		src/CGI.cpp\
		src/setupserver.cpp\


OBJS = $(SRCS:.c=.o)

CC = g++ -g --std=c++98
CFLAGS = -Wall -Werror -Wextra
NORM = norminette
RM = rm -rf

all: $(NAME)

$(OBJS_DIR)%.o:%.c
	@${CC} ${CFLAGS} -I${LIBFT_DIR} -o $@ -c $<

$(NAME):
		${CC} ${CFLAGS} ${SRCS} -o ${NAME}
#		@mv ${SRCS_DIR}*.o ${OBJS_DIR}
		@echo "____Webserv créé avec succès____"

clean:
		${RM} ${NAME} ${NAME}.dSYM
		${RM} ${OBJS_DIR}*.o
		@echo "____L'exécutable a été supprimé____"
fclean: clean

re: fclean all

.PHONY: all clean fclean re