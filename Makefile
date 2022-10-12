NAME		= webserv
CXX			= c++
CXXFLAGS	= #-std=c++98 -Wall -Wextra -Werror

SRCS		= server.cpp
OBJS		= $(SRCS:.cpp=.o)
INC			= .

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -I$(INC) -o $(NAME)

clean	:
	@rm -rf $(OBJS)

fclean	: clean
	@rm -rf $(NAME)

re		: fclean all

.PHONY	: all clean fclean re
