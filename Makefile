
CC			:= cc
CFLAGS		:= -Wextra -Wall -Werror -O3 -flto -pipe
LIB			:= -lm

# -flto = link at the end = more inlining
# -pipe = use a pipe instead of temp files during cc

ifeq ($(MAKECMDGOALS), yolo)
  $(eval CFLAGS=-O3 -flto -pipe)
endif

SRC			:=	ping_debug.c 			\
				ping_flags_parse.c		\
				ping_flags_single.c 	\
				ping_flags_word.c 		\
				ping_flags.c	 		\
				ping_init.c 			\
				ping_receive.c			\
				ping_send.c 			\
				ping_utils.c			\
				ping.c

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

flcean: fclean

re: fclean all

rebuild: re

yolo: $(OBJ) # ignore warnings
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LIB)

$(ASM_DIR)/%.s: %.c
	@mkdir -p $(@D)
	$(CC) -S $< -o $@

asm: $(ASM)
	@mkdir -p $(@D)

.PHONY: all clean fclean re

