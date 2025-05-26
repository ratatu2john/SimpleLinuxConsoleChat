
#ifndef	SIMPSOCKET_H
#define	SIMPSOCKET_H	1

void set_addrinfo(struct addrinfo** server_info, const char* domain, const char* PORT);
int bind_socket(struct addrinfo** server_info);
int connect_socket(struct addrinfo** server_info);
int get_clientfd(int sockfd, struct sockaddr_storage* peer_addr);
void* get_in_addr(struct sockaddr* sa);
int get_in_port(struct sockaddr *sa);

#endif // SIMPSOCKET_H


