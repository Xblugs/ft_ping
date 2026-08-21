
CC			:= cc
CFLAGS		:= -Wextra -Wall -Werror -O3
LIB			:=

ifeq ($(MAKECMDGOALS), yolo)
  $(eval CFLAGS=-O3)
endif

SRC			:= ping.c ping_flags.c ping_init.c ping_send.c ping_utils.c

SOURCES		:= $(SRC)

NAME		:= ft_ping
OBJ			 = $(addprefix $(OBJ_DIR)/,$(SOURCES:%.c=%.o))
OBJ_DIR		:= obj

ASM    		 = $(addprefix $(ASM_DIR)/,$(SOURCES:%.c=%.s))
ASM_DIR		:= asm

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LIB)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)
	@rm -rf $(ASM_DIR)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

rebuild: re

yolo: $(OBJ) # ignore warnings
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LIB)

$(ASM_DIR)/%.s: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -S $< -o $@

asm: $(ASM)
	@mkdir -p $(@D)

.PHONY: all clean fclean re

