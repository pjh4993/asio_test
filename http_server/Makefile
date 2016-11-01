XX=g++
CFLAGS = -I.
BOOST = -lboost_system
CTUT = -lctut
PTHREAD = -pthread
OBJ = main.o connection.o connection_manager.o mime_types.o reply.o request_handler.o request_parser.o server.o
%.o: %.cpp
	$(XX) -std=c++11 -c -o $@ $< $(CFLAGS) $(CTUT) -L /usr/local/lib 2>err.txt
main: $(OBJ)
	$(XX) -std=c++11 -o $@ $^ $(CFLAGS) $(BOOST) $(CTUT) $(PTHREAD) 2>err.txt
clean:
	rm -rf *.o

