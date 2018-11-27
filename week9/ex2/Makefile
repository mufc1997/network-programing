CC = gcc
CFLAGS = -I
LIBS = -lm
SERVER = server
CLIENT = client

all : $(SERVER) $(CLIENT)

server = $(wildcard tests/server.c)
client = $(wildcard tests/client.c)
server_obj = $(patsubst %.c,%.o,$(server))
client_obj = $(patsubst %.c,%.o,$(client))
src = $(wildcard src/*.c)
obj = $(patsubst %.c,%.o,$(src))

$(SERVER): $(server_obj) $(obj)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(CLIENT): $(client_obj) $(obj)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.o: %.c
	$(CC) -c -o $@ $< 

clean: $(server_obj) $(client_obj)
	rm -rf $(server_obj) $(client_obj) $(SERVER) $(CLIENT)