CC=c++
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src
EXEC_DIR=./exec

all: common client server


client: 
	$(CC) -c $(SRC_DIR)/client/client.c -o $(BIN_DIR)/client.o -Wall -g
	$(CC) -o $(EXEC_DIR)/client/client $(SRC_DIR)/client/main.c $(BIN_DIR)/client.o $(BIN_DIR)/common.o -g -Wall -lpthread -O0

server:
	$(CC) -c $(SRC_DIR)/server/server.c -o $(BIN_DIR)/server.o -Wall -g
	$(CC) -o $(EXEC_DIR)/server/server $(SRC_DIR)/server/main.c $(BIN_DIR)/server.o $(BIN_DIR)/common.o -g -Wall -lpthread -O0

common:
	$(CC) -c $(SRC_DIR)/common/common.c -o $(BIN_DIR)/common.o -Wall -g

clean:
	rm -rf  $(BIN_DIR)/*.o $(EXEC_DIR)/client/* $(EXEC_DIR)/server/* *~