#include <stdio.h>
#include <string.h>
#include "../lib/env.h"

//Method print error with responseCode
void errorMessage (int code) {
	char message[1024];

	switch (code) {
		case ERROR_USER:
			strcpy(message, "USER NOT EXIST\n");
			break;
		case ERROR_PASSWORD:
			strcpy(message, "PASSWORD INVALID\n");
			break;
		case ERROR_STATE:
			strcpy(message, "STATE IS WRONG\n");
			break;
		case ERROR_LOGOUT:
			strcpy(message, "YOU CANT LOG OUT\n");
			break;
		case ERROR_LOGINED:
			strcpy(message, "YOU LOGINED\n");
			break;
		case ERROR_FORM_MESSAGE:
			strcpy(message, "FORM MESSAGE INVALID\n");
			break;
		case ERROR_USER_BLOCKED:
			strcpy(message, "USER IS BLOCKED\n");
			break;
		case SUCCESS_USER:
			strcpy(message, "SUCCESS USERNAME. YOU CAN INPUT PASSWORD\n");
			break;
		case SUCCESS_PASSWORD:
			strcpy(message, "YOU LOGINED. YOU CAN LOGOUT\n");
			break;
		case SUCCESS_LOGOUT:
			strcpy(message, "LOGOUT SUCCESS\n");
			break;
	}

	printf(">>> %d - %s\n", code, message);
}