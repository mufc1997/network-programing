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

LinkList *toEndLinkList(LinkList *head) {
	if (head == NULL) {
		printf("\nLink List is null");
		return NULL;
	} else {
		LinkList *current = head;

		while (current->next != NULL) {
			current = current->next;
		}

		return current;
	}
}

void *append(LinkList **head, LinkList *node) {
	if (node == NULL) {
		printf("\nCan't append node to link list\n");
	} else if (*head == NULL){
		*head = node;
	} else {
		LinkList *current = toEndLinkList(*head);
		current->next = node;
	}
}

void *insert(LinkList **head, LinkList *node) {
	if (node == NULL) {
		printf("\nNode is NULL");
	} else if (head == NULL) {
		*head = node;
	} else {
		node->next = *head;
		*head = node;
	}
}

void *delete(LinkList **head, int status) {
	if (*head == NULL) {
		printf("\nCant delete this node");
	} else {
		LinkList *current = *head, *prev;

		while (current != NULL) {
			if (current->status == status) {
				if (current == *head) {
					*head = (*head)->next;
					free(current);
					break;
				} else if (current->next != NULL) {
					prev->next = current->next;
					current->next = NULL;
					free(current);
					break;
				} else {
					prev->next = NULL;
					free(current);
					break;
				}
			}	

			prev = current;
			current = current->next;
		}
	}
}

void printLinkList(LinkList *head) {
	if (head == NULL) {
		printf("\nLink list is NULL\n");
	} else {
		LinkList *current = head;

		while (current != NULL) {
			printf("%s\t%s\t%d\n", current->username, current->password, current->status);
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