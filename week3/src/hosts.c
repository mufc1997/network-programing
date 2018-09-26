#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../lib/hosts.h"

// Display information for domain or ip address
void display_info(enum Type type, char *official, char *alias[]) {
	char official_display[30];
	char alias_display[30];

	if(type == IP_TYPE) {
		strcpy(official_display, "Official name");
		strcpy(alias_display, "Alias name");
	} else {
		strcpy(official_display, "Official IP");
		strcpy(alias_display, "Alias IP");
	}

	printf("%s: %s\n", official_display, official);
	printf("%s: \n", alias_display);

	if(type == IP_TYPE) {
		for(int i = 0; alias[i] != NULL; i++) {
			printf("\t%s\n", alias[i]);
		}
	} else {
		for(int i = 1; alias[i] != NULL; i++) {
			printf("\t%s\n", inet_ntoa(*((struct in_addr*)alias[i])));
		}
	}
}

// Get information for ip address to display
void get_info_ip_address(char *ipAddress) {
	struct in_addr addr;
	inet_aton(ipAddress, &addr);
	struct hostent *host = gethostbyaddr(&addr, sizeof(addr), AF_INET);

	if(host == NULL) {
		printf("Not found information\n");
		return;
	}

	display_info(IP_TYPE, host->h_name, host->h_aliases);
}

// Get information for domain to display
void get_info_domain(char *domain) {
	struct hostent *host = gethostbyname(domain);

	if(host == NULL) {
		printf("Not found information\n");
		return;
	}
	char *ip_buffer = inet_ntoa(*((struct in_addr*)host->h_addr_list[0]));

	display_info(DOMAIN_TYPE, ip_buffer, host->h_addr_list);
}

// Check number between 0 and 256
int check_number(char *ip_part) {
	int number = atoi(ip_part);

	if(number < 0 || number > 256)
		return 0;
	return 1;
}

// Check ip address when input params from terminal
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