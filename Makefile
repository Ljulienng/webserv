# INCLUDES = -I includes 

#To be continued..

NAME			= 	webserv
EXEC			= 	$(addprefix ./, $(NAME))
RM				= 	/bin/rm

include webserv.mk

SRC_DIR			= 	$(shell find srcs -type d)
INC_DIR			= 	$(shell find srcs -type d)
OBJ_DIR 		=	obj

vpath %.cpp $(foreach dir, $(SRC_DIR), $(dir):)

OBJ_FILES		= 	${SRC_FILES:.cpp=.o}
OBJ 			= 	$(addprefix $(OBJ_DIR)/, $(OBJ_FILES))

#### COMPILING

CC				= 	clang++

CFLAGS			=   -Wall -Wextra -Werror --std=c++98
IFLAGS			=	$(foreach path, $(INC_DIR), -I$(path))

##### RULES

all:	${NAME}

$(OBJ_DIR):
	@mkdir -p $@

# OBJECT DIRECTORY

$(OBJ_DIR)/%.o : %.cpp $(INC_FILES)
	@echo "\r\033[KCompiling\t$< \c"
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ -c $<

# LINKING

$(NAME): $(OBJ_DIR) $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $@
	@echo "\n\033[32mOK\t\t$@ is ready !!!\033[0m"

clean:
	@$(RM) -rf $(OBJ_DIR)
	@echo "Removed\t\tobject files"

fclean: clean
	@$(RM) -f $(NAME)
	@echo "Removed\t\t$(NAME)"

re:	fclean all

docker-build:	all
				docker build -t webserver -f Dockerfile .

docker-run: 	docker-build
				docker run --rm -it --network host webserver

.PHONY:	all clean fclean re docker-build docker-run