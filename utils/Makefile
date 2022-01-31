all: install

install: utils.a

utils.a: objects
	ar -rcs utils.a obj/*.o
	ranlib utils.a

objects:
	mkdir -p obj
	clang -std=c11 -Wall -Werror -Wextra -Wpedantic `pkg-config --cflags gtk4` -c src/*.c -I inc/ -I../frameworks/openssl/ 
	mv *.o obj/
	
uninstall:
	rm -rf utils.a
	rm -rfd obj

reinstall:
	make uninstall
	make

clean:
	rm -rfd obj

