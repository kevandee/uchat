
FLAGS = -std=c11 -Wall -Werror -Wextra -Wpedantic 

all: install

install: 
	@make server

server: ../libmx/libmx.a sql_lib
	@mkdir -p obj
	@clang $(FLAGS) -c src/*.c 
	@mv *.o obj/
	@clang $(FLAGS) -g -o uchat_server obj/*.o ../frameworks/SQLite3/sql.a ../libmx/libmx.a -lpthread

sql_lib:
	@make -sC ../frameworks/SQLite3

../libmx/libmx.a:
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
