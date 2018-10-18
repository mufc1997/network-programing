#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>

#include "../lib/error.h"
#include "../lib/env.h"
#include "../lib/linklist.h"
#include "../lib/file.h"

#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 1024

//global variables
char method[30];
char value[50];

int checkUser (LinkList *root, char *username, char *password) {
	if (root == NULL) {
		return 0;
	} else {
		LinkList *current = root;

		while (current != NULL) {
			if (strcmp(current->username, username) == 0) {
				if (current->status == 0) {
					return 2; 
				}
				else if (password == NULL)
					return 1;// Username exist
				else 
					if (strcmp(current->password, password) == 0) 
						return 3;
			}
			current = current->next;
		}
	}

	return 0;
}

int split(char *recv_data) {
	char str[1024];
	strcpy(str, recv_data);
	int count = 0;
	char store[2][30];
  char *token = strtok(str, " "); 

  while (token != NULL) 
	{ 
		if (token != NULL) {
			strcpy(store[count], token);
		}

		token = strtok(NULL, " "); 
		count++;
	}

	if (count > 2) 
		return 0;

	strcpy(method, store[0]);
	strcpy(value, store[1]);
	return 1;
}

LinkList *getUser (LinkList *root, char *username, char *password) {
	LinkList *current = root;

	while (current != NULL) {
		if (password == NULL) {
			if (strcmp(current->username, username) == 0)
				return current;
		} else {
			if (strcmp(current->username, username) == 0 && strcmp(current->password, password) == 0)
				return current;
		}

		current = current->next;
	}

	return NULL;
}

void handle (LinkList **root, int *state, char *recv_data, int *responseCode, char *username, char *password) {
	LinkList *user = NULL;

	if (!split(recv_data)) {
		*responseCode = ERROR_FORM_MESSAGE;
		return;
	}

	if (*state == AUTHENTICATED) {
		if (strcmp(method, "LOGOUT") != 0) {
			*responseCode = ERROR_LOGINED;
			return;
		}
	}

	if (strcmp(method, "USER") == 0) {
		if (*state != UNDEFINE) {
			*responseCode = ERROR_STATE;
			return;
		}

		strcpy(username, value);

		int check = checkUser(*root, username, NULL);
		if (!check) {
			*responseCode = ERROR_USER;
		} else {
			if (check == 2) {
				*responseCode = ERROR_USER_BLOCKED;
				return;
			}
			*responseCode = SUCCESS_USER;
			*state = UNAUTHENTICATE;
		}
	} else if (strcmp(method, "PASS") == 0) {
		if (*state != UNAUTHENTICATE) {
			*responseCode = ERROR_STATE;
			return;
		}

		if (checkUser(*root, username, NULL) == 2) { //check user is blocked
			*responseCode = ERROR_USER_BLOCKED;
			return;
		}

		strcpy(password, value);
		if (checkUser(*root, username, password) == 3) { // login with username, password
			*responseCode = SUCCESS_PASSWORD;
			*state = AUTHENTICATED;

			user = getUser(*root, username, password);
			user->is_login = 1;
		} else {
			user = getUser(*root, username, NULL); // login error
			if (user->count > 2) { // password invalid 3 step
				user->status = 0;
				user->count = 0;
				*state = UNDEFINE;
				*responseCode = ERROR_USER_BLOCKED;
				return;
			} else {
				user->count++;
			}

			*responseCode = ERROR_PASSWORD; 
		}
	} else if (strcmp(method, "LOGOUT") == 0) {
		if (*state != AUTHENTICATED) {
			*responseCode = ERROR_STATE;
			return;
		}

		strcpy(username, value);

		if (checkUser(*root, username, NULL) == 0) {
			*responseCode = ERROR_USER;
		} else {
			*state = UNDEFINE;
			user = getUser(*root, username, password);
			user->is_login = 0;
			*responseCode = SUCCESS_LOGOUT;
		}
	} else {
		*responseCode = ERROR_FORM_MESSAGE;
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Parameter invalid\n");
	}

	// Set open port
	int PORT = atoi(argv[1]);
	int state = UNDEFINE;
	char username[30];
	char password[30];

	int listen_sock, conn_sock; /* file descriptors */
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
	char buff[BUFF_SIZE];
	int responseCode;

	LinkList *root = NULL;
	FILE *file = open("account.txt", "r+");
	readFile(file, &root);

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
			bytes_received = recv(conn_sock, buff, BUFF_SIZE, 0); //blocking
			if (bytes_received <= 0){
				printf("\nConnection closed");
				break;
			}
			else{
				handle(&root, &state, buff, &responseCode, username, password);
			}

			//echo to client
			bytes_sent = send(conn_sock, &responseCode, sizeof(responseCode), 0); /* send to the client welcome message */
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
