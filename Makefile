NAME = webserv

SRCS_DIR = src
OBJ_DIR = obj
INC_DIR = inc

SRCS = $(SRCS_DIR)/main.cpp \
       $(SRCS_DIR)/configParser.cpp \
       $(SRCS_DIR)/Handler.cpp \
       $(SRCS_DIR)/Request.cpp \
       $(SRCS_DIR)/Response.cpp \
       $(SRCS_DIR)/CGIHandler.cpp \
       $(SRCS_DIR)/autoindex.cpp \
       $(SRCS_DIR)/server/ServerConfig.cpp \
       $(SRCS_DIR)/server/Server.cpp \
       $(SRCS_DIR)/server/Client.cpp

OBJS = $(patsubst $(SRCS_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I$(INC_DIR)

.PHONY: all clean fclean re

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	@printf "  \033[33m⚙\033[0m  Compiling %d files...\n" $(words $(OBJS))
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/server

$(NAME): $(OBJS)
	@printf "  \033[32m✓\033[0m Compiled %d files → $(NAME)\n" $(words $(OBJS))
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
	@printf "  \033[31m✗\033[0m  Removing object files...\n"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@printf "  \033[31m✗\033[0m  Removing $(NAME)...\n"
	@rm -f $(NAME)

re: fclean all
