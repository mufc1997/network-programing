#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 1024

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
	int sin_size, size_file;

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
			bytes_received = recv(conn_sock, recv_data, 256, 0);

			if (bytes_received <= 0){
				printf("\nConnection closed");
				break;
			} else{
				//Check file exist on server	
				FILE *save_file = fopen(recv_data, "r");
				if (save_file != NULL) {
					bytes_sent = send(conn_sock, "exist", BUFF_SIZE, 0); /* send to the client welcome message */
					continue;
				} else {
					bytes_sent = send(conn_sock, "not exist", BUFF_SIZE, 0); /* send to the client welcome message */
				}

				//Open file when write
				save_file = fopen(recv_data, "w");
				size_file = 1;
				while(1) {
					memset(recv_data, '\0', BUFF_SIZE);

					bytes_received = recv(conn_sock, recv_data, 1024, 0); 

					if (bytes_received <= 0) {
						break;
					}

					// Size file receive
					size_file++;
					fwrite(recv_data, 1, bytes_received, save_file);

					if (bytes_received < 1024) {
						//printf("Receive file complete\n");
						bytes_sent = send(conn_sock, "Successful transfering", BUFF_SIZE, 0); /* send to the client welcome message */

						if (bytes_sent <= 0){
							printf("\nConnection closed");
							break;
						}
						break;
					} 
				}
				fclose(save_file);
			}
		}//end conversation
		close(conn_sock);	
	}
	
	close(listen_sock);
	return 0;
}
