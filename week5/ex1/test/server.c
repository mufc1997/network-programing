#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 1024

typedef struct {
	char number[50];
	char text[50];
} DataSend;

int check_string(char *str) {
	for(int i = 0; i < strlen(str) - 1; i++) {
		if (!isalnum(str[i]))
			return 0;
	}
	return 1;
}

//Split buff to number and string
DataSend split_string(char *buff) {
	int j = 0, k = 0;
	DataSend data;	

	for(int i = 0; i < strlen(buff) - 1; i++) {
		if(isdigit(buff[i]))
			data.number[j++] = buff[i];
		else if(isalpha(buff[i])) 
			data.text[k++] = buff[i];
	}

	data.number[j] = '\0';
	data.text[k] = '\0';

	return data;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Parameter invalid\n");
		return 1;
	}
	int PORT = atoi(argv[1]);
	int listen_sock, conn_sock; /* file descriptors */
	char recv_data[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
	DataSend data;
	
	//Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(PORT);   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   
	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	}     
	
	//Step 3: Listen request from client
	if(listen(listen_sock, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 4: Communicate with client
	while(1){
		//accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, &sin_size)) == -1) 
			perror("\nError: ");
  
		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); /* prints client's IP */
		
		//start conversation
		while(1){
			//receives message from client
			bytes_received = recv(conn_sock, recv_data, BUFF_SIZE-1, 0); //blocking

			if (bytes_received <= 0){
				printf("\nConnection closed");
				break;
			}
			else{
				recv_data[bytes_received] = '\0';
				data = split_string(recv_data);

				printf("\nReceive: %s ", recv_data);
			}
			
			//echo to client
			if (!check_string(recv_data)) {
				strcpy(data.text, "Error");
			}

			bytes_sent = send(conn_sock, &data, sizeof(DataSend), 0); /* send to the client welcome message */
			if (bytes_sent <= 0){
				printf("\nConnection closed");
				break;
			}
		}//end conversation
		close(conn_sock);	
	}
	
	close(listen_sock);
	return 0;
}