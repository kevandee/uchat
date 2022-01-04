
FLAGS = -std=c11 -Wall -Werror -Wextra -Wpedantic 

all: install

install: 
	@make -sC server
	@make -sC client 
	@mv client/uchat .
	@mv server/uchat_server .
clean:
	@make clean -sC server
	@make clean -sC client 

uninstall:
	@rm -rfd uchat uchat_server
	@make uninstall -sC server
	@make uninstall -sC client 

reinstall:
	@make uninstall
	@make
