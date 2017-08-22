NAME = ws_server

SRCDIR = ./srcs/

OBJDIR = ./objs/

SRC = main.c wslay_callback.c http_handshake.c tools.c child_to_parent.c \
		child_to_client.c parent_to_child.c socket.c pipe.c \
		connection_handler.c communication_handler.c environment.c

SRCS = $(addprefix $(SRCDIR), $(SRC))

OBJ = $(SRC:.c=.o)

OBJS = $(addprefix $(OBJDIR), $(OBJ))

INCS = -I ./incs

LIBFT = -L ./libft/

NETTLE_LIB_DIR = -L /lib

WSLAY_LIB_DIR = -L .libs/wslay/lib/.libs/

WSLAY_INC_DIR = -I .libs/wslay/lib/includes/

LIBS = -lnettle -lwslay -lft

CC = gcc

TAGS = -Wall -Werror -Wextra

SIZE = 4

linker:
	LD_LIBRARY_PATH=/home/mc/boris_game/server/.libs/wslay/lib/.libs
	export LD_LIBRARY_PATH

all: $(NAME)

static: $(OBJS)
	$(CC) $(TAGS) $(WSLAY_LIB_DIR) $(NETTLE_LIB_DIR) $(LIBFT) -static -o $(NAME) $(OBJS) $(LIBS)

$(NAME) : $(OBJS)
	$(CC) $(TAGS) $(WSLAY_LIB_DIR) $(NETTLE_LIB_DIR) $(LIBFT) -o $(NAME) $(OBJS) $(LIBS)

$(OBJS) : $(SRCS)
	make -C ./libft
	$(CC) $(TAGS) -DMAXPLAYER=$(SIZE) -c $(INCS) $(WSLAY_INC_DIR) $(SRCS) && mv $(OBJ) $(OBJDIR)

clean:
	make clean -C ./libft
	rm -rf $(OBJ)
	rm -rf $(OBJS)
	rm -f ./*.o

fclean: clean
	make fclean -C ./libft
	rm -rf $(NAME)

re: fclean all
