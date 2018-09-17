typedef struct LinkList{
	char username[30];
	char password[30];
	int status;
	int is_login;
	int count;
	struct LinkList *next;
} LinkList;

LinkList *newNode(char *username, char *password, int status);
LinkList *toEndLinkList(LinkList *head);
void *append(LinkList **head, LinkList *node);
void *insert(LinkList **head, LinkList*node);
void *delete(LinkList **head, int status);
void printLinkList(LinkList *head);
void destroy(LinkList **head);