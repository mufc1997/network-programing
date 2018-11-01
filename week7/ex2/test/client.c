#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h> 
#include <stdlib.h>

#include "../lib/error.h"
#include "../lib/linklist.h"
#include "../lib/file.h"
#include "../lib/env.h"

#define BUFF_SIZE 1024

//client input message
char *clientInput () {
	char *buff = (char *)malloc(sizeof(char) * BUFF_SIZE);
	memset(buff, '\0', BUFF_SIZE);
	printf("> Client input: ");
	fgets(buff, BUFF_SIZE, stdin);
	buff[strlen(buff) - 1] = '\0';
	return buff;
}

int main(int argc, char *argv[]){
	if (argc < 3) {
		printf("Parameter invalid\n");
	}

	//set server address
	char SERVER_ADDR[20]; strcpy(SERVER_ADDR, argv[1]);
	int SERVER_PORT = atoi(argv[2]);
	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received;
	
	int responseCode;
	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}
		
	//Step 4: Communicate with server			
	
	//send message
	while (1) {
		strcpy(buff, clientInput());
		if (strlen(buff) == 0) break;
			
		bytes_sent = send(client_sock, buff, BUFF_SIZE, 0);

		if(bytes_sent <= 0) {
			perror("\nError: ");
			break;
		}
		
		//receive echo reply
		bytes_received = recv(client_sock, &responseCode, sizeof(int), 0);
		if (bytes_received < 0) {
			perror("\nError: ");
			break;
		}
		else if (bytes_received == 0) {
			printf("Connection closed.\n");
			break;
		}
			
		errorMessage(responseCode);
	}

	//Step 4: Close socket
	close(client_sock);
	
	return 0;
}
