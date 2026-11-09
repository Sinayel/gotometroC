CC = cc

RM = rm -rf

CFLAGS = -Wall -Wextra -Werror

OBJ_DIR = obj
SRC_DIR = srcs

HEADER = include/header.h
SRCS = get-line-14.c 

NAME = gotometro-c

SRCS_FILES = $(addprefix $(SRC_DIR)/, $(SRCS))
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

all: $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(NAME): $(OBJS)
	@echo Compiling the project...
	@cc $(CFLAGS) $(OBJS) -lcurl -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER) | $(OBJ_DIR)
	@cc $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning objects..."
	@$(RM) $(OBJ_DIR)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re