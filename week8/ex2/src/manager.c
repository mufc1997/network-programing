#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/linklist.h"
#include "../lib/manager.h"

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