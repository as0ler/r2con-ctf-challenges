# the compiler: gcc for C program, define as g++ for C++
CC = g++

CFLAGS  = -std=c++0x -no-pie -Wall

# the build target executable:
TARGET = mmorpsg

all: $(TARGET)

$(TARGET): src/$(TARGET).cpp
	$(CC) $(CFLAGS) -o dist/$(TARGET) src/$(TARGET).cpp

clean:
	$(RM) dist/$(TARGET)
