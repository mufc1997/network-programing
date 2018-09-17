#include <stdio.h>
#include <stdlib.h>
#include "../lib/linklist.h"
#include "../lib/file.h"

FILE *open(char *filename, char *action) {
	FILE *file = fopen(filename, action);

	if (file == NULL) {
		printf("File Can\'t Open\n");
		return NULL;
	} else {
		return file;
	}
}

void *readFile(FILE *file, LinkList **head) {
	if (file == NULL) {
		printf("File can not found\n");
		return NULL;
	} else {
		char username[30], password[30];
		int status;

		while (!feof(file)) {
			fscanf(file, "%s %s %d", username, password, &status);
			append(head, newNode(username, password, status));
		}
	}
}

void *writeFile(FILE *file, LinkList *head) {
	if (file == NULL) {
		printf("File can not found\n");
	} else {
		LinkList *current = head;

		while (current != NULL) {
			fprintf(file, "%s %s %d", current->username, current->password, current->status);

			if (current->next != NULL) {
				fprintf(file, "\n");
			}

			current = current->next;
		}

		fclose(file);
	}
}
