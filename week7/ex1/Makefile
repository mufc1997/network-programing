CC = gcc
CFLAGS = -I
LIBS = -lm
SERVER = server
CLIENT = client

DEPS = $(wildcard lib/*.h)
src = $(wildcard src/*.c)
test = $(wildcard test/*.c)
lib = $(patsubst %.c,%.o,$(src))

all: $(SERVER) $(CLIENT)

$(SERVER): test/server.o $(lib)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -pthread

$(CLIENT): test/client.o $(lib)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< 

clean: $(lib) $(SERVER) $(CLIENT)
	rm -rf $(lib) $(SERVER) $(CLIENT) test/*.o