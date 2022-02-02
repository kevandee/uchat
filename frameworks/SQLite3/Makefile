
FLAGS = -std=c11 -Wall -Werror -Wextra -Wpedantic 

all: install

install: 
	@make sql_lib

sql_lib: 
	@mkdir -p obj
	@clang $(FLAGS) -c src/*.c -I inc/
	@mv *.o obj/
	@ar -rcs sql.a obj/*.o
	@ranlib sql.a

clean:
	@rm -rfd obj

uninstall:
	@rm -rf sql.a
	@rm -rfd obj

reinstall:
	@make uninstall
	@make
