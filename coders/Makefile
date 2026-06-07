NAME    = codexion
CC      = cc
CFLAGS  = -Wall -Wextra -Werror -pthread
SRCDIR  = .
SRCS    = $(SRCDIR)/main.c   \
          $(SRCDIR)/parse.c  \
		  $(SRCDIR)/logger.c  \
          $(SRCDIR)/init.c   \
          $(SRCDIR)/heap.c \
          $(SRCDIR)/dongle.c \
          $(SRCDIR)/coder.c \
          $(SRCDIR)/monitor.c 
OBJS    = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re