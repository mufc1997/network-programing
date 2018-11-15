#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/linklist.h"


LinkList *newNode(char *username, char *password, int status) {
	LinkList *node = (LinkList *)malloc(sizeof(LinkList));
	strcpy(node->username, username);
	strcpy(node->password, password);
	node->status = status;
	return node;
}

LinkList *isExist (LinkList *head, LinkList *nodeSearch) {
	if (head == NULL) {}
	else {
		LinkList *current = head;

		while (current != NULL) {
			if (strcmp(current->username, nodeSearch->username) == 0) {
				return current;
			}

			current = current->next;
		}
	}

	return NULL;
}

LinkList *insert(LinkList **head, LinkList *node) {
	if (node == NULL) {
		printf("\nNode is NULL");
	} else if (*head == NULL) {
		*head = node;
	} else {
		if (isExist(*head, node) != NULL) {
			return NULL;
		}
		LinkList *current = *head;

		while (current->next != NULL) {
			current = current->next;
		}
		
		current->next = node;
	}

	return *head;
}

void printLinkList(LinkList *head) {
	if (head == NULL) {
		printf("\nLink list is NULL\n");
	} else {
		LinkList *current = head;

		while (current != NULL) {
			printNode(current);
			current = current->next;
		}
	}
}

void printNode(LinkList *node) {
	if (node == NULL) {
		printf("\nNode is NULL");
	} else {
		printf("\nUsername: %s\nPassword: %s\nStatus: %d\n", node->username, node->password, node->status);
	}
}

void destroy(LinkList **head) {
	if (*head == NULL) {
		return;
	}

	LinkList *current = *head, *prev;

	while (current != NULL) {
		if (current->next == NULL) {
			free(current);
			prev->next = NULL;
			current = *head;
		}

		prev = current;
		current=current->next;
	}

	free(*head);

	*head = NULL;
}