
FLAGS = -std=c11 -Wall -Werror -Wextra -Wpedantic 

all: 
	@rm -rfd uchat uchat_server kevandee/uchat Dima123/uchat Fibbs123/uchat
	@make install

install: 
	@make -sC server
	@make -sC client 
	@mv client/uchat .
	@mv server/uchat_server .

client:
	@make -sC client 
	@mv client/uchat .

clean:
	@make clean -sC server
	@make clean -sC client 

uninstall:
	@rm -rfd uchat uchat_server kevandee/uchat Dima123/uchat Fibbs123/uchat
	@make uninstall -sC server
	@make uninstall -sC client 

reinstall:
	@make uninstall
	@make
