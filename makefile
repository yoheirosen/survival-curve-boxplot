CWD:=$(shell pwd)

SRC_DIR:=$(CWD)/src
BIN_DIR:=$(CWD)/bin
OBJ_DIR:=$(CWD)/obj

CXX:=g++
CXXFLAGS:=-std=c++11 -g

all : tests

build_dirs:
	if [ ! -d $(OBJ_DIR) ]; then mkdir -p $(OBJ_DIR); fi
	if [ ! -d $(BIN_DIR) ]; then mkdir -p $(BIN_DIR); fi

clean:
	rm $(BIN_DIR)/* $(OBJ_DIR)/*.o

tests : $(OBJ_DIR)/tests.o $(OBJ_DIR)/banddepth.o
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/tests

$(OBJ_DIR)/tests.o : $(SRC_DIR)/tests.cpp $(SRC_DIR)/banddepth.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/banddepth.o : $(SRC_DIR)/banddepth.cpp $(SRC_DIR)/banddepth.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
