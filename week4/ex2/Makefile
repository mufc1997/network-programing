CC = gcc
CFLAGS = -I
LIBS = -lm
SERVER = server
CLIENT = client

all : $(SERVER) $(CLIENT)

DEPS = $(wildcard lib/*.h)
src = $(wildcard src/*.c)
obj = $(patsubst %.c,%.o,$(src))

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< 

$(SERVER): $(obj) test/server.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(CLIENT): $(obj) test/client.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean: $(obj)
	rm -rf $(obj) $(SERVER) $(CLIENT) test/server.o test/client.o
