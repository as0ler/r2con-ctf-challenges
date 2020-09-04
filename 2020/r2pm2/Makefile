CC = gcc
CFLAGS  = -s -Wall

TARGET = r2pm2

all: $(TARGET)

$(TARGET): src/$(TARGET).c
	$(CC) $(CFLAGS) -o dist/$(TARGET) src/$(TARGET).c

clean:
	$(RM) dist/$(TARGET)

deploy:
	./deploy.sh

stop:
	docker stop $(docker ps -a -q)
