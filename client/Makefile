
FLAGS = -std=c11 -Wall -Werror -Wextra -Wpedantic 

all: install

install: 
	@make client 

client: libmx/libmx.a sql_lib
	@mkdir -p obj
	@clang $(FLAGS) `pkg-config --cflags gtk+-3.0` -DGDK_DISABLE_DEPRECATION_WARNINGS -c src/*.c 
	@mv *.o obj/
	@clang $(FLAGS) -g -o uchat obj/*.o `pkg-config --libs gtk+-3.0` ../libmx/libmx.a ../frameworks/SQLite3/sql.a -lpthread
sql_lib:
	@make -sC ../frameworks/SQLite3

libmx/libmx.a:
	@make -sC ../libmx

clean:
	@make clean -sC ../libmx
	@make clean -sC ../frameworks/SQLite3
	@rm -rfd obj

uninstall:
	@rm -rf uchat 
	@rm -rfd obj
	@make uninstall -sC ../libmx
	@make uninstall -sC ../frameworks/SQLite3

reinstall:
	@make uninstall
	@make
