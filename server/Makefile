
FLAGS = -std=c11 -Wall -Werror -Wextra -Wpedantic 

all: install

install: 
	@make server

server: ../libmx/libmx.a sql_lib ../utils/utils.a
	@mkdir -p obj
	@clang $(FLAGS) `pkg-config --cflags gtk4` -c src/*.c -I../frameworks/openssl/
	@mv *.o obj/
	@clang $(FLAGS)   -g -o uchat_server obj/*.o ../utils/utils.a ../frameworks/SQLite3/sql.a ../libmx/libmx.a ../frameworks/openssl/libcrypto.a ../frameworks/openssl/libssl.a -lcurl -lpthread 

sql_lib:
	@make -sC ../frameworks/SQLite3

../libmx/libmx.a:
	@make -sC ../libmx

../utils/utils.a:
	@make -sC ../utils

clean:
	@make clean -sC ../utils
	@make clean -sC ../libmx
	@rm -rfd obj

uninstall:
	@rm -rf uchat_server
	@rm -rfd obj
	@make uninstall -sC ../utils
	@make uninstall -sC ../libmx

reinstall:
	@make uninstall
	@make
