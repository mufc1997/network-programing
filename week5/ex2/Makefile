CC = gcc
CFLAGS = -I
LIBS = -lm
SERVER = server
CLIENT = client

all: $(SERVER) $(CLIENT)
DEPS = $(wildcard lib/*.h)

$(SERVER): test/server.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(CLIENT): test/client.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< 

clean: $(SERVER) $(CLIENT) test/*.o
	rm -rf $(SERVER) $(CLIENT) test/*.o