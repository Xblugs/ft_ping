
CC			:= cc
CFLAGS		:= -Wextra -Wall -Werror -O3
LIB			:=

SRC			:= ping.c ping_flags.c

SOURCES		:= $(SRC)

NAME		:= ft_ping
OBJ			 = $(addprefix $(OBJ_DIR)/,$(SOURCES:.c=.o))
OBJ_DIR		:= obj

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LIB)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re

