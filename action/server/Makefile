XX=g++
CFLAGS = -I.
BOOST = -lboost_system
CTUT = -lctut
PTHREAD = -pthread
MYSQL = -lmysqlcppconn
OBJ = main.o connection.o connection_manager.o reply.o request_handler.o request_parser.o server.o
%.o: %.cpp
	$(XX) -std=c++11 -c -o $@ $< $(CFLAGS) $(CTUT) $(MYSQL) -L /usr/local/lib 2>err.txt
main: $(OBJ)
	$(XX) -std=c++11 -o $@ $^ $(CFLAGS) $(BOOST) $(CTUT) $(MYSQL) $(PTHREAD) 2>err.txt
clean:
	rm -rf *.o

