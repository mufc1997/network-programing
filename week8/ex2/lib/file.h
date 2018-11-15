FILE *open(char *filename, char *action);
void *readFile(FILE *file, LinkList **head);
void *writeFile(FILE *file, char *file_name, LinkList *head);