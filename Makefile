TARGET = exchange_matcher

SRC = $(wildcard src/*.c)

OBJ = $(wildcard *.o)

CC = clang
CFLAGS = -Wall -Wextra -Wpedantic -Werror -Wshadow

${TARGET}:
	${CC} ${CFLAGS} -o $@ ${SRC}

.PHONY: all clean fclean re

all: ${TARGET}

clean:
	rm -rf ${OBJ}

fclean: clean
	rm -rf ${TARGET}

re: fclean all