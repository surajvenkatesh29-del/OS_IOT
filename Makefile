CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
LDFLAGS = -lgpiod -lpthread
TARGET = bin/traffic_signal

all: $(TARGET)

$(TARGET): src/traffic_signal.c include/traffic_signal.h
	mkdir -p bin
	$(CC) $(CFLAGS) src/traffic_signal.c -o $(TARGET) $(LDFLAGS)

simulation: simulation/traffic_simulation.c
	mkdir -p bin
	$(CC) -Wall -Wextra -O2 simulation/traffic_simulation.c -o bin/traffic_simulation

clean:
	rm -rf bin

.PHONY: all simulation clean
