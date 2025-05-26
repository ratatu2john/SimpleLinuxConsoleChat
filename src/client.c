#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include "lib/simpsocket.h"

#define PORT "7777"

int setup_client(const char* hostname);
void handle_connection(int sockfd);


int main(int argc, char* argv[]) {
	int sockfd;
	
	if ( argc != 2 ) {
		fprintf(stderr, "Usage: client <hostname>\n");
		return 1;
	}
	
	sockfd = setup_client(argv[1]);	
	handle_connection(sockfd);
	
	return 0;
}



void handle_connection(int sockfd) {
	const int BUF_SIZE = 1024;
	char buf[BUF_SIZE];
	const char exit_condition[] = "exit";
	int message_size;
	
	printf("--> Write [exit] to stop chat\n"
			"--> Waiting response from server...\n\n");
			
	while (1) {
		memset(buf, 0, BUF_SIZE);
		if ((message_size = recv(sockfd, buf, BUF_SIZE-1, 0)) == -1) {
			perror("Error in recv()");
			exit(EXIT_FAILURE);
		}
		buf[message_size] = '\0';
		printf("Server: %s", buf);
		
		printf("Client: ");
		memset(buf, 0, BUF_SIZE);
		fgets(buf, BUF_SIZE-1, stdin);
		if (send(sockfd, buf, strlen(buf), 0) == -1) {
			perror("Error in recv()");
			exit(EXIT_FAILURE);
		}
			
		
		if ( strncmp(buf, exit_condition, sizeof(exit_condition)-1) == 0) {
			break;
		}
	}
	close(sockfd);
}



int setup_client(const char* hostname) {
	int sockfd;
	struct addrinfo* server_info;
	char ip[INET6_ADDRSTRLEN];
	
	set_addrinfo(&server_info, hostname, PORT); // func from simpsocket.h
	sockfd = connect_socket(&server_info); // func from simpsocket.h
	
	inet_ntop(server_info->ai_family,
				get_in_addr((struct sockaddr*)server_info->ai_addr), 
				ip, sizeof(ip));
	printf("--> Client connected to %s:%d\n",
			ip, get_in_port(server_info->ai_addr));
	freeaddrinfo(server_info);
	
	return sockfd;
}
