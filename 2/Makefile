CC = g++
BUILD_DIR = build
CFLAGS = -std=c++17

MAIN_EXECUTABLE_FILE = main.out
REDUCE_EXECUTABLE_FILE = reduce.out
MAP_EXECUTABLE_FILE = map.out


MAIN_OBJECTS = main.o

REDUCE_OBJECTS = reduce.o

MAP_OBJECTS = map.o 

MainSensitivityList = main.cpp

MapSensitivityList = map.cpp

ReducetSensitivityList = reduce.cpp 

all: $(MAIN_EXECUTABLE_FILE) $(REDUCE_EXECUTABLE_FILE) $(MAP_EXECUTABLE_FILE)

main.o: $(MainSensitivityList)
	$(CC) $(CFLAGS) -c main.cpp -o main.o

$(MAIN_EXECUTABLE_FILE): $(MAIN_OBJECTS)
	$(CC) $(CFLAGS) $(MAIN_OBJECTS) -o $(MAIN_EXECUTABLE_FILE)

map.o: $(MapSensitivityList)
	$(CC) $(CFLAGS) -c map.cpp -o map.o

$(MAP_EXECUTABLE_FILE): $(MAP_OBJECTS)
	$(CC) $(CFLAGS) $(MAP_OBJECTS) -o $(MAP_EXECUTABLE_FILE)

reduce.o: $(ReducetSensitivityList)
	$(CC) $(CFLAGS) -c reduce.cpp -o reduce.o

$(REDUCE_EXECUTABLE_FILE): $(REDUCE_OBJECTS)
	$(CC) $(CFLAGS) $(REDUCE_OBJECTS) -o $(REDUCE_EXECUTABLE_FILE)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.o *.out