#include<stdio.h>
#include "../lib/linklist.h"

void menu() {
	printf("USER MANAGEMENT PROGRAM\n");
	printf("---------------------------------------------\n");
	printf("\t1. Register\n");
	printf("\t2. Sign in\n");
	printf("\t3. Search\n");
	printf("\t4. Sign out\n");
	printf("Your choice (1-4, other to quit):\t");
}

int main() {
	int active = 1, select;

	do {
		menu();
		scanf("%d", &select);
		
		switch (select) {
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			default:
				printf("\nYour choice error\n");
				active = 0;
				break;
		}
	} while (active == 1);

	return 1;
}