NAME = ush

FLAGS = -std=c11 -Wall -Werror -Wextra -Wpedantic

all: install

install: OBJECTS libmx/libmx.a
	@clang $(FLAGS) -g -o $(NAME) obj/*.o libmx/libmx.a

OBJECTS:
	@mkdir -p obj
	@clang $(FLAGS) -c src/*.c
	@mv *.o obj/

libmx/libmx.a: 
	@make -sC libmx

clean:
	@make clean -sC libmx
	@rm -rfd obj

uninstall:
	@rm -rf $(NAME)
	@rm -rfd obj
	@make uninstall -sC libmx

reinstall:
	@make uninstall
	@make
