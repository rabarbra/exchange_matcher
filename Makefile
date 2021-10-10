TARGET = exchange_matcher

MAIN = main.c
SRC = src/*.c
OBJ = $(SRC:.c=.o)
INCLUDES = includes/*.h

CC = clang
CFLAGS = -Wall -Wextra -Wpedantic -Werror -Wshadow

.PHONY: all clean fclean re

${OBJ}:
	${CC} ${CFLAGS} -c ${MAIN}

${TARGET}:${OBJ}
	${CC} ${CFLAGS} -o ${TARGET} $(MAIN:.c=.o)

all: ${TARGET}

clean:
	rm -rf ${OBJ} $(MAIN:.c=.o)

fclean: clean
	rm -rf ${TARGET}

re: fclean all