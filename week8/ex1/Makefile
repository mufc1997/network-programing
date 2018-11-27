CC = gcc
CFLAGS = -I
LIBS = -lm
SERVER = server
CLIENT = client

DEPS = $(wildcard lib/*.h)
src = $(wildcard src/*.c)
tests = $(wildcard tests/*.c)
libs = $(patsubst %.c,%.o,$(src))

all: $(SERVER) $(CLIENT)

$(SERVER): tests/server.o $(libs)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(CLIENT): tests/client.o $(libs)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< 

clean: $(libs) $(SERVER) $(CLIENT)
	rm -rf $(libs) $(SERVER) $(CLIENT) tests/*.o