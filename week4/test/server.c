/*UDP Echo Server*/
#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define BEGIN_NUMBER 48
#define END_NUBMER 57
#define BEGIN_STRING_UP 65
#define END_STRING_UP 90
#define BEGIN_STRING_DOWN 97
#define END_STRING_DOWN 122

#define BUFF_SIZE 1024
int check_string(char *str) {
	for(int i = 0; i < strlen(str) - 1; i++) {
		if (!isalnum(str[i]))
			return 0;
	}
	return 1;

}

void split_string(char *main_string, char number[], char str[]) {
	int j = 0, k = 0;
	strcpy(number, "");
	strcpy(str, "");

	for(int i = 0; i < strlen(main_string) - 1; i++) {
		if(isdigit(main_string[i]))
			number[j++] = main_string[i];
		else if(isalpha(main_string[i])) 
			str[k++] = main_string[i];
	}

	number[j] = '\0';
	str[k] = '\0';
}

int main(int argc, char *argv[])
{
	if(argc < 2) {
		printf("Not found PORT");
		exit(0);
	}

	int PORT = atoi(argv[1]);
	int server_sock; /* file descriptors */
	char buff[BUFF_SIZE], number[BUFF_SIZE], str[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	struct sockaddr_in cliaddr;
	int sin_size;

	//Step 1: Construct a UDP socket
	if ((server_sock=socket(AF_INET, SOCK_DGRAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		exit(0);
	}
	
	//Step 2: Bind address to socket
	server.sin_family = AF_INET;         
	server.sin_port = htons(PORT);   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = INADDR_ANY;  /* INADDR_ANY puts your IP address automatically */   
	bzero(&(server.sin_zero),8); /* zero the rest of the structure */

  
	if(bind(server_sock,(struct sockaddr*)&server,sizeof(struct sockaddr))==-1){ /* calls bind() */
		perror("\nError: ");
		exit(0);
	}     
	
	//Step 3: Communicate with clients
	while(1){
		sin_size=sizeof(struct sockaddr_in);
    		
		bytes_received = recvfrom(server_sock, buff, BUFF_SIZE-1, 0, (struct sockaddr *) &client, &sin_size);
		
		if (bytes_received < 0)
			perror("\nError: ");
		else{
			buff[bytes_received] = '\0';
			printf("[%s:%d]: %s", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), buff);
		}

		if(check_string(buff) == 1) {
			split_string(buff, number, str);

			bytes_sent = sendto(server_sock, number, bytes_received, 0, (struct sockaddr *) &client, sin_size ); /* send to the client welcome message */
			
			bytes_sent = sendto(server_sock, str, bytes_received, 0, (struct sockaddr *) &client, sin_size ); /* send to the client welcome message */
		} else {
			strcpy(buff, "Error");

			bytes_sent = sendto(server_sock, buff, bytes_received, 0, (struct sockaddr *) &client, sin_size ); /* send to the client welcome message */
		}

		if (bytes_sent < 0)
			perror("\nError: ");					
	}
	
	close(server_sock);
	return 0;
}
