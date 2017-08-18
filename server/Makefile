NAME = server

SRCDIR = ./srcs/

OBJDIR = ./objs/

SRC = main.c wslay_callback.c http_handshake.c tools.c

SRCS = $(addprefix $(SRCDIR), $(SRC))

OBJ = $(SRC:.c=.o)

OBJS = $(addprefix $(OBJDIR), $(OBJ))

INCS = -I ./incs

WSLAY_LIB_DIR = -L .libs/wslay/lib/.libs/

WSLAY_INC_DIR = -I .libs/wslay/lib/includes/

LIBS = -lnettle -lwslay

CC = gcc

TAGS = -Wall -Werror -Wextra

all: $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(TAGS) -g -o $(NAME) $(WSLAY_LIB_DIR) $(LIBS) $(OBJS)

$(OBJS) : $(SRCS)
	$(CC) $(TAGS) -g -c $(INCS) $(WSLAY_INC_DIR) $(SRCS)
	mv $(OBJ) $(OBJDIR)

clean:
	rm -rf $(OBJ)
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all
