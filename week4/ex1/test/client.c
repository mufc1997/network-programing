/*UDP Echo Client*/
#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFF_SIZE 1024

// receive buff socker from server
int recv_socket(int client_sock, char *buff, struct sockaddr *server_addr, int *sin_size) {
	int bytes_received;

	bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, server_addr, sin_size);

	if(strcmp(buff, "Error") == 0)
		return 0;

	if(bytes_received < 0){
		perror("Error: ");
		close(client_sock);
		return 0;
	}
	buff[bytes_received] = '\0';

	// if buff null not print
	if(strcmp(buff, "") != 0)
		printf("\t%s\n", buff);

	return 1;
}

int main(int argc, char *argv[]){
	if (argc < 3) {
		printf("Not enough params\n");
		exit(0);
	}

	char SERV_IP[30];
	strcpy(SERV_IP, argv[1]);
	int SERV_PORT = atoi(argv[2]);
	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr;
	int bytes_sent, sin_size;
	
	//Step 1: Construct a UDP socket
	if ((client_sock=socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){  /* calls socket() */
		perror("\nError: ");
		exit(0);
	}

	//Step 2: Define the address of the server
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERV_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERV_IP);
	
	//Step 3: Communicate with server
	while(1) {
		printf("INPUT: ");
		memset(buff,'\0',(strlen(buff)+1));
		fgets(buff, BUFF_SIZE, stdin);

		// if buff null stop application
		if(strcmp(buff, "\n") == 0) {
			return 1;
		}

		sin_size = sizeof(struct sockaddr);
			
		bytes_sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *) &server_addr, sin_size);
		if(bytes_sent <= 1){
			perror("Error: ");
			close(client_sock);
			return 0;
		}

		printf("OUTPUT:\n");
		if(recv_socket(client_sock, buff, (struct sockaddr *) &server_addr, &sin_size) == 1) {
			recv_socket(client_sock, buff, (struct sockaddr *) &server_addr, &sin_size);
		} else {
			printf("\t%s\n", buff);
			close(client_sock);
			return 0;
		}

	};

	close(client_sock);
	return 0;
}
