#include <stdio.h>
#include <stdlib.h>
#include "../lib/linklist.h"
#include "../lib/file.h"
#include "../lib/manager.h"

void menu() {
	printf("\nUSER MANAGEMENT PROGRAM\n");
	printf("---------------------------------------------\n");
	printf("\t1. Register\n");
	printf("\t2. Sign in\n");
	printf("\t3. Search\n");
	printf("\t4. Sign out\n");
	printf("Your choice (1-4, other to quit):\t");
}

int main() {
	int active = 1, select;
	FILE *file = open("account.txt", "r+");
	LinkList *list = NULL, *userLogin = NULL;
	readFile(file, &list);

	do {
		menu();
		scanf("%d", &select);

		switch (select) {
			case 1: {
				char username[30], password[30];

				printf("Username: "); scanf("%s", username);

				if (search(list, username) != NULL) {
					printf("Account existed\n");
					break;
				}

				printf("Password: "); scanf("%s", password);

				append(&list, newNode(username, password, 1));

				printf("Successful registration\n");

				writeFile(file, list);
				break;
			}

			case 2: {
				char username[30], password[30];
				LinkList *node = NULL;

				printf("Username: "); scanf("%s", username);

				if ((node = search(list, username)) == NULL) {
					printf("Cannot find account\n");
					break;
				} else {
					if (node->status == 0) {
						printf("Account is blocked\n");
						break;
					}
				}

				printf("Password: "); scanf("%s", password);
				checkLogin(&list, username, password);

				writeFile(file, list);
				break;
			}

			case 3: {
				LinkList *node = NULL;

				if ((node = beforeUseApp(list)) == NULL) {
					printf("Cannot find account\n");
				} else {
					if (node->status == 1) {
						printf("Account is active\n");
					} else {
						printf("Account is blocked\n");
					}
				}

				break;
			}

			case 4: {
				LinkList *node = NULL;

				if ((node = beforeUseApp(list)) == NULL) {
					printf("Cannot find account\n");
				} else {
					if (node->status == 0) {
						printf("Account is blocked\n");
					} else {
						if (node->is_login == 0) {
							printf("Account is not sign in\n");
						} else {
							printf("Goodbye %s\n", node->username);
							node->is_login = 0;
						}
					}
				}

				break;
			}
			default: {
				writeFile(file, list);
				destroy(&list);
				fclose(file);
				printf("\nYour choice error\n");
				active = 0;
				break;
			}
		}
	} while (active == 1);

	return 1;
}