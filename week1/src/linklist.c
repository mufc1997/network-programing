#include <stdio.h>
#include <stdlib.h>
#include "../lib/linklist.h"


LinkList *newNode(int status) {
	LinkList *node = (LinkList *)malloc(sizeof(LinkList));
	node->status = status;
	return node;
}

LinkList *toEndLinkList(LinkList *head) {
	if (head == NULL) {
		printf("Link List is null");
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
		printf("Can't append node to link list\n");
	} else if (*head == NULL){
		*head = node;
	} else {
		LinkList *current = toEndLinkList(*head);
		current->next = node;
	}
}

void *insert(LinkList **head, LinkList *node) {
	if (node == NULL) {
		printf("node is NULL");
	} else if (head == NULL) {
		*head = node;
	} else {
		node->next = *head;
		*head = node;
	}
}

void *delete(LinkList **head, int status) {
	if (*head == NULL) {
		printf("Cant delete this node");
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
		printf("Link list is NULL\n");
	} else {
		LinkList *current = head;

		while (current != NULL) {
			printf("Status: %d\n", current->status);
			current = current->next;
		}
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