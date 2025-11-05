CXX      := g++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98 -Iinclude -g

NAME     := webserv
SRC_DIR  := src
OBJ_DIR  := obj

SRCS := main.cpp \
        configParser.cpp \
		Handler.cpp \
		Request.cpp \
		Response.cpp \
        CGIHandler.cpp \
        autoindex.cpp \
		server/ServerConfig.cpp \
        server/Server.cpp \
		server/Client.cpp \
		

SRCS := $(addprefix $(SRC_DIR)/, $(SRCS))
OBJS := $(addprefix $(OBJ_DIR)/, $(SRCS:$(SRC_DIR)/%.cpp=%.o))

all: $(NAME)

$(NAME): $(OBJS)
	@printf "\033[1;32m[✔] Linking:\033[0m %s\n" "$(NAME)"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@printf "\033[1;34m[🛠] Building:\033[0m %s\n" "$<"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@printf "\033[1;33m[🧹] Cleaning object files...\033[0m\n"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@printf "\033[1;31m[🔥] Removing binary:\033[0m %s\n" "$(NAME)"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
