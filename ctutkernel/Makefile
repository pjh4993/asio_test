all:
	g++ -Wall -shared -fpic -o dll/libctut.so src/util/log.cpp -Iheader/
	sudo rm -rf /usr/local/lib/libctut.so
	sudo mkdir -p /usr/local/include/ctutkernel
	sudo rm -rf /usr/local/include/ctutkernel/log.hpp
	sudo cp dll/libctut.so /usr/local/lib/libctut.so
	sudo ln header/*.hpp /usr/local/include/ctutkernel/
clean:
	sudo rm -rf /usr/local/lib/libctut.so
