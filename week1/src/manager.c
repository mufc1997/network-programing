#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/linklist.h"
#include "../lib/manager.h"

int checkLogin(LinkList **head, char *username, char *password) {
	if (*head == NULL) {
		printf("List empty\n");
		return 0;
	} else {
		LinkList *node = NULL;

		if ((node = search(*head, username)) == NULL) {
			printf("Cannot find account\n");
			return 0;
		} else {
			if (node->status == 0) {
				printf("Account is blocked\n");
				return 0;
			}
		} 
		LinkList *current = *head;

		while (current != NULL) {
			if(strcmp(current->username, username) == 0 && strcmp(current->password, password) == 0) {
				if (current->is_login == 0) {
					printf("Hello %s\n", current->username);
					current->is_login = 1;
					current->count = 0;
					return 1;
				} else {
					printf("Account is logined\n");
					return 0;
				}
			} else if (strcmp(current->username, username) == 0 && strcmp(current->password, password) != 0){
				if(current->count == 1) {
					printf("Password is incorrect. Account is blocked\n");
					current->is_login = 0;
					current->status = 0;
					return 0;
				}

				printf("Password is incorrect\n");
				current->count++;
			}

			current = current->next;
		}

		return 0;
	}
}

LinkList *search(LinkList *head, char *username) {
	if (head == NULL) {
		printf("List empty");
		return NULL;
	} else {
		LinkList *current = head;

		while (current != NULL) {
			if(strcmp(current->username, username) == 0) {
				return current;
			}

			current = current->next;
		}

		return NULL;
	}
}

LinkList *beforeUseApp(LinkList *list) {
	LinkList *node = NULL;
	char username[30];

	printf("Username: "); scanf("%s", username);

	return search(list, username);
}