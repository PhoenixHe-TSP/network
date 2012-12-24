CXX= clang++
CXXFLAGS= -std=c++11 -pipe -O0
LDFLAGS= -lstdc++ -lpthread -lboost_system -lboost_thread
VPATH= src

OBJ= server client connection traffic_monitor
OBJ:= $(addsuffix .o,$(OBJ))
BIN= client server

.PHONY:all clean

all: $(BIN)

server: connection.o server.o
client: traffic_monitor.o connection.o client.o

clean:
	rm -f $(OBJ) $(BIN)
