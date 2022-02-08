
FLAGS = -std=c11 -Wall -Werror -Wextra -Wpedantic 

all: install

install: 
	@make client 

client: ../libmx/libmx.a sql_lib ../utils/utils.a
	@mkdir -p obj
	@clang $(FLAGS) `pkg-config --cflags gtk4`    -c src/*.c  -I../frameworks/openssl/
	@mv *.o obj/
	@clang $(FLAGS) `pkg-config --cflags gtk4`  -g -o uchat obj/*.o  `pkg-config --libs gtk4` ../utils/utils.a ../libmx/libmx.a ../frameworks/SQLite3/sql.a ../frameworks/openssl/libcrypto.a ../frameworks/openssl/libssl.a  -lpthread -lcurl 
sql_lib:
	@make -sC ../frameworks/SQLite3

../libmx/libmx.a:
	@make -sC ../libmx

../utils/utils.a:
	@make -sC ../utils

clean:
	@make clean -sC ../utils
	@make clean -sC ../libmx
	@make clean -sC ../frameworks/SQLite3
	@rm -rfd obj

uninstall:
	@rm -rf uchat 
	@rm -rfd obj
	@make uninstall -sC ../utils
	@make uninstall -sC ../libmx
	@make uninstall -sC ../frameworks/SQLite3

reinstall:
	@make uninstall
	@make
