#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>

#define BUFF_SIZE 8192

int send_file_to_server(int client_sock, char *path) {
	char *buff = (char *)malloc(sizeof(char) * 1024);
	int bytes_received;
	memset(buff, '\0', 1024);

	FILE *file;
	if ((file = fopen(path, "r")) == NULL) {
		printf("> Error: File not found\n");
		return 0;
	} else {
		send(client_sock, basename(path), 256, 0);
		bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);

		//Check response from server file exist on server
		if(strcmp(buff, "exist") == 0) {
			printf("> Error: File is existent on server\n");
			return 0;
		}

		while(1) {
			memset(buff, '\0', 1024);

			int nread = fread(buff, 1, 1024, file);

			if (nread > 0) {
				send(client_sock, buff, nread, 0);
			}

			if (nread < 1024) {
				if(feof(file)) {
					printf("Read file complete\n");
					break;
				}
			}
		}

		fclose(file);
		return 1;
	}
}

int main(int argc, char *argv[]){
	// Check parameter
	if (argc != 3) {
		printf("Parameter invalid\n");
		return 1;
	}

	int client_sock;
	char SERVER_ADDR[30];
	strcpy(SERVER_ADDR, argv[1]);
	int SERVER_PORT = atoi(argv[2]);
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received;
	
	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("> Error!Can not connect to sever! Client exit imediately!\n");
		return 0;
	}
		
	//Step 4: Communicate with server			
	while(1){
		//Input path file
		printf(">>> Input file path: ");
		memset(buff,'\0',BUFF_SIZE);
		fgets(buff, BUFF_SIZE, stdin);	

		//Remove enter symbol in word end
		buff[strlen(buff) - 1] = '\0';

		//input length
		msg_len = strlen(buff);
		if (msg_len == 0) {
			break;
		}
		//Sent file to server		
		bytes_sent = send_file_to_server(client_sock, buff);

		if(bytes_sent == 0){
			continue;
		}
		
		//receive echo reply
		bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
		if(bytes_received <= 0){
			printf("> Error!Cannot receive data from sever!\n");
			break;
		}
		
		printf("> Reply from server: %s\n", buff);
	}
	
	//Step 4: Close socket
	close(client_sock);
	return 0;
}
