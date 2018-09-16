typedef struct LinkList{
	char *username;
	char *password;
	int status;
	int is_login;
	struct LinkList *next;
} LinkList;

LinkList *newNode();
LinkList *toEndLinkList(LinkList *head);
void *append(LinkList **head, LinkList*node);
void *insert(LinkList **head, LinkList*node);
void *delete(LinkList **head, int status);
void printLinkList(LinkList *head);
void destroy(LinkList **head);