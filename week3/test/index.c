#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
enum Type {
	IP_TYPE,
	DOMAIN_TYPE
};

void display_info(enum Type type, char *official, char *alias) {
	char official_display[30];

	if(type == IP_TYPE) {
		strcpy(official_display, "Official name");
	} else {
		strcpy(official_display, "Official IP");
	}

	printf("%s: %s\n", official_display, official);
}

void get_info_ip_address(char *ipAddress) {
	struct in_addr addr;
	inet_aton(ipAddress, &addr);
	struct hostent *host = gethostbyaddr(&addr, sizeof(addr), AF_INET);

	if(host == NULL) {
		printf("Not found information\n");
		return;
	}

	display_info(IP_TYPE, host->h_name, NULL);
}

void get_info_domain(char *domain) {
	struct hostent *host = gethostbyname(domain);

	if(host == NULL) {
		printf("Not found information\n");
		return;
	}
	char *ip_buffer = inet_ntoa(*((struct in_addr*)host->h_addr_list[0]));

	display_info(DOMAIN_TYPE, ip_buffer, NULL);
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