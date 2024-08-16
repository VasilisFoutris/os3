# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -IHeaderFiles

# Source and header directories
SRC_DIR = CodeFiles
HDR_DIR = HeaderFiles

# Object files
OBJS = $(SRC_DIR)/main.o $(SRC_DIR)/client.o $(SRC_DIR)/dispatcher.o $(SRC_DIR)/server.o

# Executable name
TARGET = main

# Rule to compile and link the program
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rules to compile individual source files into object files
$(SRC_DIR)/main.o: $(SRC_DIR)/main.cpp $(HDR_DIR)/client.h $(HDR_DIR)/dispatcher.h $(HDR_DIR)/server.h
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/main.cpp -o $(SRC_DIR)/main.o

$(SRC_DIR)/client.o: $(SRC_DIR)/client.cpp $(HDR_DIR)/client.h
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/client.cpp -o $(SRC_DIR)/client.o

$(SRC_DIR)/dispatcher.o: $(SRC_DIR)/dispatcher.cpp $(HDR_DIR)/dispatcher.h
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/dispatcher.cpp -o $(SRC_DIR)/dispatcher.o

$(SRC_DIR)/server.o: $(SRC_DIR)/server.cpp $(HDR_DIR)/server.h
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/server.cpp -o $(SRC_DIR)/server.o

# Clean up the build
clean:
	rm -f $(SRC_DIR)/*.o $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)
