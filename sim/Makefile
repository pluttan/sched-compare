CXX ?= c++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic

SRC := task.h scheduler.h cfs.h cfs.cpp eevdf.h eevdf.cpp trace.h trace.cpp runner.cpp main.cpp
OBJ := cfs.o eevdf.o trace.o runner.o main.o
BIN := sim

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

cfs.o: cfs.cpp cfs.h scheduler.h task.h
eevdf.o: eevdf.cpp eevdf.h scheduler.h task.h
trace.o: trace.cpp trace.h task.h
runner.o: runner.cpp scheduler.h task.h
main.o: main.cpp trace.h cfs.h eevdf.h scheduler.h task.h

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean
