#include <stdio.h>
#include <stdlib.h>
#include "../lib/hosts.h"

int main(char argc, char *argv[]) {
	// Check params exist
	if(argv[1] == NULL) {
		printf("Params not found\n");
		exit(1);
	}

	char *params = argv[1];
	
	// params is ip address or domain 
	if(ip_valid(params)) {
		get_info_ip_address(params);
	} else {
		get_info_domain(params);
	}

	return 1;
}