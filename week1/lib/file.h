FILE *open(char *filename, char *action);
void *readFile(FILE *file, LinkList **head);
void *writeFile(FILE *file, LinkList *head);