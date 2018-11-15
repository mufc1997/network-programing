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
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <sys/time.h>
#include <string.h>
#include <arpa/inet.h>

#include "../lib/error.h"
#include "../lib/env.h"
#include "../lib/linklist.h"
#include "../lib/file.h"
				
#define BACKLOG 20   /* Number of allowed connections */
#define BUFF_SIZE 1024

//global variables
char method[30];
char value[50];
char username[FD_SETSIZE][30];
char password[FD_SETSIZE][30];

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
			if (strcmp(current->username, username) == 0 && strcmp(current->password, password) == 0) {//get with username and password 
				return current;
			}
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
/* The processData function copies the input string to output */
void processData(char *in, char *out);

/* The recv() wrapper function*/
int receiveData(int s, char *buff, int size, int flags);

/* The send() wrapper function*/
int sendData(LinkList *root, int *state, int responseCode, int s, char *buff, int size, int flags, char *username, char *password);

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Parameter invalid\n");
		return 1;
	}
	// Set open port
	int PORT = atoi(argv[1]);

	int i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready, client[FD_SETSIZE];
	ssize_t	ret;
	fd_set	readfds, allset;
	char sendBuff[BUFF_SIZE], rcvBuff[BUFF_SIZE];
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	//Step 1: Construct a TCP socket to listen connection request
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}

	//Step 2: Bind address to socket
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(PORT);

	if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	} 

	//Step 3: Listen request from client
	if(listen(listenfd, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}

	int state[FD_SETSIZE];
	maxfd = listenfd;			/* initialize */
	maxi = -1;				/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++) {
		client[i] = -1;			/* -1 indicates available entry */
		state[i] = UNDEFINE;
	}
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	//Step 4: Communicate with clients
	while (1) {
		readfds = allset;		/* structure assignment */
		nready = select(maxfd+1, &readfds, NULL, NULL, NULL);
		if(nready < 0){
			perror("\nError: ");
			return 0;
		}
		
		if (FD_ISSET(listenfd, &readfds)) {	/* new client connection */
			clilen = sizeof(cliaddr);
			if((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0)
				perror("\nError: ");
			else{
				printf("You got a connection from %s\n", inet_ntoa(cliaddr.sin_addr)); /* prints client's IP */
				for (i = 0; i < FD_SETSIZE; i++)
					if (client[i] < 0) {
						client[i] = connfd;	/* save descriptor */
						break;
					}
				if (i == FD_SETSIZE){
					printf("\nToo many clients");
					close(connfd);
				}

				FD_SET(connfd, &allset);	/* add new descriptor to set */
				if (connfd > maxfd)
					maxfd = connfd;		/* for select */
				if (i > maxi)
					maxi = i;		/* max index in client[] array */

				if (--nready <= 0)
					continue;		/* no more readable descriptors */
			}
		}

		for (i = 0; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &readfds)) {
				ret = receiveData(sockfd, rcvBuff, BUFF_SIZE, 0);
				if (ret <= 0){
					FD_CLR(sockfd, &allset);
					close(sockfd);
					client[i] = -1;
					state[i] = UNDEFINE;
				}
				
				else {
					int responseCode;
					LinkList *root = NULL;
					FILE *file = open("account.txt", "r+");
					readFile(file, &root);

					processData(rcvBuff, sendBuff);

					sendData(root, &state[i], responseCode, sockfd, sendBuff, sizeof(responseCode), 0, username[i], password[i]);
					if (ret <= 0){
						FD_CLR(sockfd, &allset);
						close(sockfd);
						client[i] = -1;
					}
				}

				if (--nready <= 0)
					break;		/* no more readable descriptors */
			}
		}
	}
	
	return 0;
}

void processData(char *in, char *out){
	strcpy (out, in);
}

int receiveData(int s, char *buff, int size, int flags){
	int n;
	n = recv(s, buff, size, flags);
	if(n < 0)
		perror("Error: ");
	return n;
}

int sendData(LinkList *root, int *state, int responseCode, int s, char *buff, int size, int flags, char *username, char *password){
	int n;

	handle(&root, state, buff, &responseCode, username, password);
	n = send(s, &responseCode, size, flags);
	if(n < 0)
		perror("Error: ");
	return n;
}
