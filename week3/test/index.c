#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void display_info(char *official_ip, char *alias) {
	printf("Official IP: %s\n", official_ip);
}

void get_info_ip_address(char *ipAddress) {
	struct in_addr addr;
	inet_aton(ipAddress, &addr);
	struct hostent *host = gethostbyaddr(&addr, sizeof(addr), AF_INET);

	display_info(host->h_name, NULL);
}

void get_info_domain(char *domain) {
	struct hostent *host = gethostbyname(domain);
	char *ip_buffer = inet_ntoa(*((struct in_addr*)host->h_addr_list[0]));

	display_info(ip_buffer, NULL);
}

int check_number(char *ip_part) {
	int number = atoi(ip_part);

	if(number < 0 || number > 256)
		return 0;
	return 1;
}

int ip_valid(char *ip) {
	char str[40];
	strcpy(str, ip);

	int count = 0;
	int number = 0;

  char *token = strtok(str, "."); 

  while (token != NULL) 
	{ 
		if(!check_number(token))
			return 0;
		token = strtok(NULL, "."); 
		count++;
	}

	if(count != 4) 
		return 0;

	printf("count: %d\n", count);
	return 1;
}

int main(char argc, char *argv[]) {
	if(argv[1] == NULL) {
		printf("Params not found\n");
		exit(1);
	}

	char *params = argv[1];

	if(ip_valid(params)) {
		get_info_ip_address(params);
	} else {
		get_info_domain(params);
	}

	return 1;
}