#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <errno.h>
#include <arpa/inet.h>

#include "../lib/error.h"
#include "../lib/env.h"
#include "../lib/linklist.h"
#include "../lib/file.h"
				
#define BACKLOG 20
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
					return 2; //user is blocked
				}
				else if (password == NULL)
					return 1;// Username exist
				else 
					if (strcmp(current->password, password) == 0) 
						return 3; //User logined success
			}
			current = current->next;
		}
	}

	return 0;
}

//Split message to method and value
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

//Get information user by username, password
LinkList *getUser (LinkList *root, char *username, char *password) {
	LinkList *current = root;

	while (current != NULL) {
		if (password == NULL) {
			if (strcmp(current->username, username) == 0) //get with only username
				return current;
		} else {
			if (strcmp(current->username, username) == 0 && strcmp(current->password, password) == 0)//get with username and password
				return current;
		}

		current = current->next;
	}

	return NULL;
}

//handle message to responseCode send to client
void handle (LinkList **root, int *state, char *recv_data, int *responseCode, char *username, char *password) {
	LinkList *user = NULL;

	if (!split(recv_data)) {
		*responseCode = ERROR_FORM_MESSAGE;
		return;
	}

	// Alert user logined
	if (*state == AUTHENTICATED) {
		if (strcmp(method, "LOGOUT") != 0) {
			*responseCode = ERROR_LOGINED;
			return;
		}
	}

	// method is user
	if (strcmp(method, "USER") == 0) {
		if (*state != UNDEFINE) {
			*responseCode = ERROR_STATE; //error state
			return;
		}

		strcpy(username, value);
		
		//check user exist
		int check = checkUser(*root, username, NULL);
		if (!check) {
			*responseCode = ERROR_USER;
		} else {
			if (check == 2) { //check user is blocked
				*responseCode = ERROR_USER_BLOCKED;
				return;
			}
			*responseCode = SUCCESS_USER; //alert user input user success
			*state = UNAUTHENTICATE; //to next state
		}
	} else if (strcmp(method, "PASS") == 0) { //method is pass
		if (*state != UNAUTHENTICATE) { 
			*responseCode = ERROR_STATE; //error state
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

			user = getUser(*root, username, password); //logined
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

		char oldUsername[50];
		strcpy(oldUsername, username);
		strcpy(username, value);

		//Check user exist
		if (!checkUser(*root, username, NULL)) {
			strcpy(username, oldUsername);
			*responseCode = ERROR_USER;
		} else {
			if (strcmp(username, oldUsername) != 0) {
				strcpy(username, oldUsername);
				*responseCode = ERROR_LOGOUT;
				return;
			}

			*state = UNDEFINE; //state to first state
			user = getUser(*root, username, password);
			user->is_login = 0;
			*responseCode = SUCCESS_LOGOUT; //log out success
		}
	} else {
		*responseCode = ERROR_FORM_MESSAGE;
	}
}
/* Handler process signal*/
void sig_chld(int signo);

/*
* Receive and echo message to client
* [IN] sockfd: socket descriptor that connects to client 	
*/
void echo(int conn_sock, LinkList *root, int state, int responsecode, FILE *file);

int main(int argc, char *argv[]){
		if (argc < 2) {
		printf("Parameter invalid\n");
	}

	// Set open port
	int PORT = atoi(argv[1]);
	int listen_sock, conn_sock; /* file descriptors */
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	pid_t pid;
	int sin_size;

	if ((listen_sock=socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		printf("socket() error\n");
		return 0;
	}
	
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   

	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ 
		perror("\nError: ");
		return 0;
	}     

	if(listen(listen_sock, BACKLOG) == -1){  
		perror("\nError: ");
		return 0;
	}
	
	/* Establish a signal handler to catch SIGCHLD */
	signal(SIGCHLD, sig_chld);

	while(1){
		sin_size=sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size))==-1){
			if (errno == EINTR)
				continue;
			else{
				perror("\nError: ");			
				return 0;
			}
		}
		
		/* For each client, fork spawns a child, and the child handles the new client */
		pid = fork();
		
		/* fork() is called in child process */
		if(pid  == 0){
			close(listen_sock);

			int state = UNDEFINE, responseCode;

			LinkList *root = NULL;
			FILE *file = open("account.txt", "r+");
			readFile(file, &root);

			printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */
			echo(conn_sock, root, state, responseCode, file);					
			exit(0);
		}
		
		/* The parent closes the connected socket since the child handles the new client */
		close(conn_sock);
	}
	close(listen_sock);
	return 0;
}

void sig_chld(int signo){
	pid_t pid;
	int stat;
	
	/* Wait the child process terminate */
	while((pid = waitpid(-1, &stat, WNOHANG))>0)
		printf("\nChild %d terminated\n",pid);
}

void echo(int conn_sock, LinkList *root, int state, int responseCode, FILE *file) {
	int bytes_received, bytes_sent;
	char buff[BUFF_SIZE];
	char username[30];
	char password[30];
	while (1) {
		bytes_received = recv(conn_sock, buff, BUFF_SIZE, 0); //blocking
		if (bytes_received <= 0){
			printf("\nConnection closed");
			break;
		}
		else{
			handle(&root, &state, buff, &responseCode, username, password);
			writeFile(file, "account.txt", root);
		}

		//echo to client
		bytes_sent = send(conn_sock, &responseCode, sizeof(responseCode), 0); /* send to the client welcome message */
		if (bytes_sent <= 0){
			printf("\nConnection closed");
			break;
		}
	}

	close(conn_sock);
}
