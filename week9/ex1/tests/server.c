#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/uio.h>

#define TRUE 1
#define FALSE 0
#define BACKLOG 32
#define BUFFSIZE 1024

/**
 * Method name
 **/

int convert(char *source, char *number, char *text);

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Params invalid\n");
		exit(0);
	}

	int SERVER_PORT = atoi(argv[1]);
	int i, len, rc, on = 1;
	int listen_sd, max_sd, new_sd;
	int desc_ready, end_server = FALSE;
	int close_conn;
	char buffer[BUFFSIZE];
	struct sockaddr_in addr;
	struct timeval timeout;
	fd_set master_set, working_set;
	struct iovec iov[2];
	char number[BUFFSIZE];
	char text[BUFFSIZE];
	iov[0].iov_base = number;
	iov[0].iov_len = sizeof(number);
	iov[1].iov_base = text;
	iov[1].iov_len = sizeof(text);

	listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sd < 0)
	{
		perror("socket() failed");
		exit(0);
	}

	/*************************************************************/
	/* Set socket to be nonblocking. All of the sockets for    */
	/* the incoming connections will also be nonblocking since  */
	/* they will inherit that state from the listening socket.   */
	/*************************************************************/
	rc = ioctl(listen_sd, FIONBIO, (char *)&on);
	if (rc < 0)
	{
		perror("ioctl() failed");
		close(listen_sd);
		exit(0);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(SERVER_PORT);
	rc = bind(listen_sd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0)
	{
		perror("bind() failed");
		close(listen_sd);
		exit(0);
	}

	rc = listen(listen_sd, BACKLOG);
	if (rc < 0)
	{
		perror("listen() failed");
		close(listen_sd);
		exit(0);
	}

	/*************************************************************/
	/* Initialize the master fd_set                              */
	/*************************************************************/
	FD_ZERO(&master_set);
	max_sd = listen_sd;
	FD_SET(listen_sd, &master_set);

	/*************************************************************/
	/* Initialize the timeval struct to 3 minutes.  If no        */
	/* activity after 3 minutes this program will end.           */
	/*************************************************************/
	timeout.tv_sec = 3 * 60;
	timeout.tv_usec = 0;

	/*************************************************************/
	/* Loop waiting for incoming connects or for incoming data   */
	/* on any of the connected sockets.                          */
	/*************************************************************/
	do
	{
		memcpy(&working_set, &master_set, sizeof(master_set));

		/**********************************************************/
		/* Call select() and wait 3 minutes for it to complete.   */
		/**********************************************************/
		printf("Waiting on select()...\n");
		rc = select(max_sd + 1, &working_set, NULL, NULL, &timeout);

		if (rc < 0)
		{
			perror("  select() failed");
			break;
		}

		/**********************************************************/
		/* Check to see if the 3 minute time out expired.         */
		/**********************************************************/
		if (rc == 0)
		{
			printf("  select() timed out.  End program.\n");
			break;
		}

		desc_ready = rc;
		for (i = 0; i <= max_sd && desc_ready > 0; ++i)
		{
			if (FD_ISSET(i, &working_set))
			{
				desc_ready -= 1;

				/****************************************************/
				/* Check to see if this is the listening socket     */
				/****************************************************/
				if (i == listen_sd)
				{
					printf("Listening socket is readable\n");
					do
					{
						/**********************************************/
						/* Accept each incoming connection.  If       */
						/* accept fails with EWOULDBLOCK, then we     */
						/* have accepted all of them.  Any other      */
						/* failure on accept will cause us to end the */
						/* server.                                    */
						/**********************************************/
						new_sd = accept(listen_sd, NULL, NULL);
						if (new_sd < 0)
						{
							if (errno != EWOULDBLOCK)
							{
								perror("  accept() failed");
								end_server = TRUE;
							}
							break;
						}

						printf("  New incoming connection - %d\n", new_sd);
						FD_SET(new_sd, &master_set);
						if (new_sd > max_sd)
							max_sd = new_sd;

					} while (new_sd != -1);
				}

				/****************************************************/
				/* This is not the listening socket, therefore an   */
				/* existing connection must be readable             */
				/****************************************************/
				else
				{
					printf("  Descriptor %d is readable\n", i);
					close_conn = FALSE;
					/*************************************************/
					/* Receive all incoming data on this socket      */
					/* before we loop back and call select again.    */
					/*************************************************/
					// do
					// {
					/**********************************************/
					/* Receive data on this connection until the  */
					/* recv fails with EWOULDBLOCK.  If any other */
					/* failure occurs, we will close the          */
					/* connection.                                */
					/**********************************************/
					rc = recv(i, buffer, sizeof(buffer), 0);
					buffer[rc] = '\0';

					if (rc < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							perror("  recv() failed");
							close_conn = TRUE;
						}
					}

					else if (rc == 0)
					{
						printf("  Connection closed\n");
						close_conn = TRUE;
					}
					else
					{

						/**********************************************/
						/* Data was received                          */
						/**********************************************/
						len = rc;
						printf("  %d bytes received\n", len);

						/**********************************************/
						/* Echo the data back to the client           */
						/**********************************************/
						/**
						 * Convert receive data to number and text
						 **/
						int isSuccessConvert = convert(buffer, number, text);

						if (isSuccessConvert == 0)
						{
							printf("error\n");
						}
						else
						{
							printf("Number: %s\n", number);
							printf("Text: %s\n", text);
						}

						rc = writev(i, iov, 2);

						memset(number, '\0', BUFFSIZE);
						memset(text, '\0', BUFFSIZE);
						memset(buffer, '\0', BUFFSIZE);
						if (rc < 0)
						{
							perror("  send() failed");
							close_conn = TRUE;
						}
					}
					// } while (TRUE);

					if (close_conn)
					{
						close(i);
						FD_CLR(i, &master_set);
						if (i == max_sd)
						{
							while (FD_ISSET(max_sd, &master_set) == FALSE)
								max_sd -= 1;
						}
					}
				} /* End of existing connection is readable */
			}		/* End of if (FD_ISSET(i, &working_set)) */
		}			/* End of loop through selectable descriptors */

	} while (end_server == FALSE);

	/*************************************************************/
	/* Clean up all of the sockets that are open                  */
	/*************************************************************/
	for (i = 0; i <= max_sd; ++i)
	{
		if (FD_ISSET(i, &master_set))
			close(i);
	}
	return 0;
}

int convert(char *source, char *number, char *text)
{
	int len = strlen((const char *)source);
	memset(number, '\0', BUFFSIZE);
	memset(text, '\0', BUFFSIZE);
	if (len <= 0)
	{
		return 0;
	}

	int id_number = 0, id_text = 0;

	for (int i = 0; i < len - 1; i++)
	{
		if (isdigit(source[i]))
		{
			number[id_number++] = source[i];
		}
		else if (isalpha(source[i]))
		{
			text[id_text++] = source[i];
		}
		else
		{
			strcpy(text, "Error");
			strcpy(number, "");
			return 0;
		}
	}

	return 1;
}