TARGET = exchange_matcher

SRC = main.c \
	  src/*.c

CC = clang
CFLAGS = -Wall -Wextra -Wpedantic -Werror -Wshadow

${TARGET}:${OBJ}
	${CC} ${CFLAGS} -o ${TARGET} ${SRC}

.PHONY: all clean fclean re

all: ${TARGET}

clean:
	rm -rf *.o

fclean: clean
	rm -rf ${TARGET}

re: fclean all