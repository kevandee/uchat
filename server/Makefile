
FLAGS = -std=c11 -Wall -Werror -Wextra -Wpedantic 

all: install

install: 
	@make server

server: libmx/libmx.a
	@mkdir -p obj
	@clang $(FLAGS) -c src/*.c 
	@mv *.o obj/
	@clang $(FLAGS) -g -o uchat_server obj/*.o ../libmx/libmx.a -lpthread

libmx/libmx.a:
	@make -sC ../libmx

clean:
	@make clean -sC ../libmx
	@rm -rfd obj

uninstall:
	@rm -rf uchat_server
	@rm -rfd obj
	@make uninstall -sC ../libmx

reinstall:
	@make uninstall
	@make
