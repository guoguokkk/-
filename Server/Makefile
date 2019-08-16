CC = g++
TARGET = runServer
BIN = bin
MEMORY = memory
SERVER = server
TEST = test

SRC = $(wildcard $(MEMORY)/*.cpp $(SERVER)/*.cpp $(TEST)/*.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC))

#addprefix 加前缀
#notdir 去掉目录，取文件名
$(TARGET) : $(OBJ)
	$(CC) $(addprefix $(BIN)/, $(notdir $^)) -pthread -o $(BIN)/$(TARGET)

%.o : %.cpp
	$(CC) -g -c $< -o $(BIN)/$(notdir $@)

.PHONY : clean
clean :
	rm $(BIN)/$(TARGET) -f
	rm $(BIN)/*.o -f