NAME = server

SRCDIR = ./srcs/

OBJDIR = ./objs/

SRC = main.c

SRCS = $(addprefix $(SRCDIR), $(SRC))

OBJ = $(SRC:.c=.o)

OBJS = $(addprefix $(OBJDIR), $(OBJ))

INCS = -I ./incs

LIBS = #here add few libraries

CC = gcc

TAGS = -Wall -Werror -Wextra

all: $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(TAGS) -o $(NAME) $(LIBS) $(OBJS)

$(OBJS) : $(SRCS)
	$(CC) $(TAGS) -c $(INCS) $(SRCS)
	mv $(OBJ) $(OBJDIR)

clean:
	rm -rf $(OBJ)
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all
