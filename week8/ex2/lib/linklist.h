typedef struct LinkList{
	char username[30];
	char password[30];
	int status;
	int is_login;
	int count;
	struct LinkList *next;
} LinkList;

LinkList *newNode(char *username, char *password, int status);
LinkList *isExist (LinkList *head, LinkList *nodeSearch);
LinkList *insert(LinkList **head, LinkList*node);
void printLinkList(LinkList *head);
void printNode(LinkList *node);
void destroy(LinkList **head);