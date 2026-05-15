NAME    = codexion
CC      = cc
CFLAGS  = -Wall -Wextra -Werror -pthread
SRCDIR  = coders
SRCS    = $(SRCDIR)/main.c   \
          $(SRCDIR)/time.c \
          $(SRCDIR)/parse.c  \
          $(SRCDIR)/init.c   \
		  $(SRCDIR)/stop.c  \
          $(SRCDIR)/coder.c \
          $(SRCDIR)/monitor.c \
          $(SRCDIR)/heap.c 
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