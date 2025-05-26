#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "simpsocket.h"



void set_addrinfo(struct addrinfo** server_info, const char* domain, const char* port) {
	struct addrinfo hints = {0};
	int status;
	
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if (domain == 0) { // server needs flags
		hints.ai_flags = AI_PASSIVE;
	}
	// fill server_info struct
	if ((status = getaddrinfo(domain, port, &hints, server_info)) != 0) {
		fprintf(stderr, "Error in getaddrinfo(): %s\n", gai_strerror(status));
	}
}




int connect_socket(struct addrinfo** server_info) {
	int sockfd;
	
	for (; *server_info != 0; *server_info = (*server_info)->ai_next) {
		if ( (sockfd = socket((*server_info)->ai_family, 
				(*server_info)->ai_socktype, (*server_info)->ai_protocol)) == -1) {
			perror("Error in socket()");
			continue;
		}
		if ( (connect(sockfd, (*server_info)->ai_addr, 
				(*server_info)->ai_addrlen)) == -1) { 
			close(sockfd);
			perror("Error in connect()");
			continue;
		}
		break;
	}
	
	
	if (*server_info == 0) {
		fprintf(stderr, "Client failed to connect\n");
		exit(EXIT_FAILURE);
	}
	return sockfd;
}




int bind_socket(struct addrinfo** server_info) {
	int yes = 1;
	int sockfd;
	
	
	for (; *server_info != 0 ; *server_info = (*server_info)->ai_next) {
		if ( (sockfd = socket((*server_info)->ai_family, 
				(*server_info)->ai_socktype, (*server_info)->ai_protocol)) == -1) {
			perror("Error in socket()");
			continue;
		}
		// enable address reuse to avoid bind() errors
		if ( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
				&yes, sizeof(yes)) == -1 ) {
			perror("Error in setsockopt()");
			exit(EXIT_FAILURE);
		}
		if ( bind(sockfd, (*server_info)->ai_addr, 
				(*server_info)->ai_addrlen) == -1) {
			perror("Error in bind()");
			continue;
		}
		break;
	}
	
	if (*server_info == 0) {
		fprintf(stderr,"Server failed to bind\n");
		exit(EXIT_FAILURE);
	}
	
	return sockfd;
}




int get_clientfd(int sockfd, struct sockaddr_storage* peer_addr) {
	int clientfd;
	socklen_t addr_size;
	
	
	addr_size = sizeof(peer_addr);
	clientfd = accept(sockfd, (struct sockaddr*)peer_addr, &addr_size);
	
	if ( clientfd == -1) {
		perror("Error in accept()");
		exit(EXIT_FAILURE);
	}
	
	return clientfd;
}



void* get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



int get_in_port(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return ntohs(((struct sockaddr_in*)sa)->sin_port);
	}

	return ntohs(((struct sockaddr_in6*)sa)->sin6_port);
}
