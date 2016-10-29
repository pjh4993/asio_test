all:
	g++ -Wall -shared -fpic -o dll/libctut.so src/util/log.cpp -Iheader/
	sudo rm -rf /usr/local/lib/libctut.so
	sudo rm -rf /usr/local/include/ctutkernel/log.hpp
	sudo ln dll/libctut.so /usr/local/lib/libctut.so
	sudo ln header/*.hpp /usr/local/include/ctutkernel/
clean:
	sudo rm -rf /usr/local/lib/libctut.so
