#include<stdio.h>
#include "../lib/linklist.h"

int main() {
	LinkList *head = NULL;

	printLinkList(head);

	append(&head, newNode(2));

	append(&head, newNode(3));

	append(&head, newNode(4));
	
	insert(&head, newNode(1));

	delete(&head, 1);

	printLinkList(head);

	destroy(&head);

	printLinkList(head);

	return 1;
}