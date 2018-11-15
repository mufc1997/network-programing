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
#include <poll.h>

#define BACKLOG 20
#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Params invalid\n");
		return 0;
	}

	int listenfd, connfd, sockfd;
	struct sockaddr_in cliaaddr, servaddr;
	int PORT = atoi(argv[1]);
	struct pollfd sockets[1024];
	int socknum = 1;
	char buff[BUFF_SIZE];
	int sin_size;
	memset(buff, '\0', BUFF_SIZE);

	int nfds = 1, current_size = 0, i, j;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("\nError: ");
		return 0;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
	{
		perror("\nError: ");
		return 0;
	}

	if (listen(listenfd, BACKLOG) == -1)
	{
		perror("\nError: ");
		return 0;
	}

	sockets[0].fd = listenfd;
	sockets[0].events = POLLIN;

	while (1)
	{
		int ret = poll(sockets, socknum + 1, 5000);

		if (ret == -1)
		{
			perror("\nError: ");
			break;
		}
		else if (ret == 0)
		{
			printf("Time out\n");
			break;
		}
		else
		{
			for (int i = 0; i < socknum; i++)
			{
				if (sockets[i].revents == 0)
				{
					continue;
				}

				if (sockets[i].revents != POLLIN)
				{
					printf("  Error! revents = %d\n", sockets[i].revents);
					break;
				}

				if (sockets[i].revents & POLLIN)
				{
					printf("  Listening socket is readable\n");

					connfd = accept(listenfd, (struct sockaddr *)&cliaaddr, &sin_size);
					if (connfd < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							perror("  accept() failed");
						}
						break;
					}
					else
					{
						if (socknum == 1024)
						{
							printf("Too many client\n");
							return 0;
						}
						else
						{
							for (int i = 0; i < 1024; i++)
							{
								if (sockets[i].fd == -1)
								{
									sockets[i].fd = connfd;
									sockets[i].events = POLLIN;
									socknum++;
								}
							}
						}
					}
				}
				if (sockets[i].fd > -1 && sockets[i].revents & POLLIN)
				{
					printf("  New incoming connection - %d\n", connfd);
					recv(connfd, buff, BUFF_SIZE, 0);
					sockets[i].fd = -1;
					socknum--;
					printf("BUff: %s\n", buff);
					send(connfd, buff, BUFF_SIZE, 0);
				}
			}
		}
	}
	return 1;
}