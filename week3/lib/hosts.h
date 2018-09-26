// Const type for ip address or domain
enum Type {
	IP_TYPE,
	DOMAIN_TYPE
};

// function in application
void display_info(enum Type type, char *official, char *alias[]);
void get_info_ip_address(char *ipAddress);
void get_info_domain(char *domain);
int check_number(char *ip_part);
int ip_valid(char *ip);
