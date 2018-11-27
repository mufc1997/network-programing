#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../libs/hosts.h"

// Ghep xau ki tu
void string_cat(enum Type type, char *official, char *alias[], char *buff) {
	char official_display[100];
	char alias_display[1024];

	if(type == IP_TYPE) {
		strcpy(official_display, "Official name:\t");
		strcpy(alias_display, "Alias name:\n");
	} else {
		strcpy(official_display, "Official IP:\t");
		strcpy(alias_display, "Alias IP:\n");
	}
	
	strcat(official_display, official);
	strcat(official_display, "\n");

	if(type == IP_TYPE) {
		for(int i = 0; alias[i] != NULL; i++) {
			strcat(alias_display, alias[i]);
			strcat(alias_display, "\n");
		}
	} else {
		for(int i = 1; alias[i] != NULL; i++) {
			strcat(alias_display, inet_ntoa(*((struct in_addr*)alias[i])));
			strcat(alias_display, "\n");
		}
	}

	strcat(official_display, alias_display);
	strcpy(buff, official_display);
}

// Get information for ip address to display
int get_info_ip_address(char *ipAddress) {
	struct in_addr addr;
	inet_aton(ipAddress, &addr);
	struct hostent *host = gethostbyaddr(&addr, sizeof(addr), AF_INET);

	if(host == NULL) {
		return 0;
	}
	
	string_cat(IP_TYPE, host->h_name, host->h_aliases, ipAddress);
	return 1;
}

// Get information for domain to display
int get_info_domain(char *domain) {
	domain[strlen(domain) - 1] = '\0';

	struct hostent *host = gethostbyname(domain);

	if(host == NULL) {
		return 0;
	}
	
	char *ip_buffer = inet_ntoa(*((struct in_addr*)host->h_addr_list[0]));
	string_cat(DOMAIN_TYPE, ip_buffer, host->h_addr_list, domain);

	return 1;
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