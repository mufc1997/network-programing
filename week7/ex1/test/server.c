#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdlib.h>

#define BACKLOG 20   /* Number of allowed connections */
#define BUFF_SIZE 1024

typedef struct {
	int opCode;
	int length;
	char payload[1019];
} Code;

/* Receive and echo message to client */
void *echo(void *);

Code split(char *buff) {
	Code code;
	buff[1] = '\0';
	code.opCode = atoi(buff);

	return code;
}
int main(int argc, char *argv[])
{ 
	if (argc < 2) {
		printf("Parameter invalid\n");
	}

	// Set open port
	int PORT = atoi(argv[1]);

	int listenfd, *connfd;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
	pthread_t tid;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(PORT); 
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   

	if(bind(listenfd,(struct sockaddr*)&server, sizeof(server))==-1){ 
		perror("\nError: ");
		return 0;
	}     

	if(listen(listenfd, BACKLOG) == -1){  
		perror("\nError: ");
		return 0;
	}
	
	sin_size=sizeof(struct sockaddr_in);
	while(1){		
		connfd = malloc(sizeof(int));
		if ((*connfd = accept(listenfd, (struct sockaddr *)&client, &sin_size)) ==- 1)
			perror("\nError: ");
				
		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); /* prints client's IP */
		
		/* For each client, spawns a thread, and the thread handles the new client */
		pthread_create(&tid, NULL, &echo, connfd);	
	}
	
	close(listenfd);
	return 0;
}

void *echo(void *arg){
	int connfd;
	int bytes_sent, bytes_received;
	char buff[BUFF_SIZE + 1];
	Code code;

	connfd = *((int *) arg);
	free(arg);
	pthread_detach(pthread_self());
	while (1) {
		bytes_received = recv(connfd, buff, BUFF_SIZE, 0); //blocking
		code = split(buff);
		if (bytes_received < 0) {
			perror("\nError: ");
			break;
		} else if (bytes_received == 0) {
			printf("Connection closed.");
			break;
		}

		printf("opcode: %d\n", code.opCode);

		bytes_sent = send(connfd, buff, bytes_received, 0); /* send to the client welcome message */
		if (bytes_sent < 0){
			perror("\nError: ");
			break;
		}
	}

	close(connfd);	
}
