#include <wait.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "lib/simpsocket.h"

#define PORT "7777"
#define BACKLOG 5


void sigchld_handler(int sig);
void setup_signchld_handler();
int setup_server();
void handle_client(int clientfd, int client_num);
void start_server(int sockfd);


int main() {
	int sockfd;
	
	sockfd = setup_server();
	// handle child process termination to avoid zombies processes
	setup_signchld_handler();
	start_server(sockfd);
	
	return 0;
}




void start_server(int sockfd) {
	int clientfd;
	pid_t pid;
	char ip[INET6_ADDRSTRLEN];
	struct sockaddr_storage client_addr;
	int client_num = 0;
	
	printf("--> Waiting for connections...\n");
	while (1) {
		// func from simpsocket.h
		clientfd = get_clientfd(sockfd, &client_addr);
		++client_num;
		pid = fork();
		
		if ( pid == 0) { // child process
			close(sockfd); // don't need it in child process
			
			handle_client(clientfd, client_num);	
			
			printf("--> Client closed connection\n");
			
			// clientfd needs to be closed, or will continue 
			// to exist even if we close it in parent process
			close(clientfd);
			exit(EXIT_SUCCESS);
		} else if ( pid > 0) {
			close(clientfd); // don't need it in parent process
			
			// Network byte order (Big-endian) ->
			// Host byte order (On my pc Little-endian)
			inet_ntop(client_addr.ss_family,
				get_in_addr((struct sockaddr*)&client_addr), 
				ip, sizeof(ip));
				
			printf("\n--> Child process %d(Client%d) started\n",
					pid, client_num);
			printf("--> Server got connection from %s:%d\n", 
					ip, get_in_port((struct sockaddr*)&client_addr));
			
		} else {
			perror("Error in fork()");
			exit(EXIT_FAILURE);
		}
	}
}

void handle_client(int clientfd, int client_num) {
	const int BUF_SIZE = 1024;
	const char exit_condition[] = "exit";
	char buf[BUF_SIZE];
	int message_size;
	
	printf("\n");
	while (1) {
		printf("Server->Client%d: ", client_num);
		memset(buf, 0, BUF_SIZE);
		fgets(buf, BUF_SIZE-1, stdin);
		if (send(clientfd, buf, strlen(buf), 0) == -1) {
			perror("Error in send()");
		}
		
		
		memset(buf, 0, BUF_SIZE);
		message_size = recv(clientfd, buf, BUF_SIZE-1, 0);
		if (message_size == -1) {
			perror("Error in recv()");
		}
		
		buf[message_size] = '\0';
		if ( strncmp(buf, exit_condition, sizeof(exit_condition)-1) == 0) {
			break;
		}
		printf("Client%d->Server: %s", client_num, buf);
	}
}




int setup_server() {
	struct addrinfo* server_info;
	int sockfd;
	char ip[INET6_ADDRSTRLEN];
	
	set_addrinfo(&server_info, 0, PORT); // func from simpsocket.h
	sockfd = bind_socket(&server_info); // func from simpsocket.h
	
	inet_ntop(server_info->ai_family, 
		get_in_addr(server_info->ai_addr), ip, sizeof(ip));
	printf("Server binded to %s:%d\n",
			ip, get_in_port(server_info->ai_addr));
			
	// no need in this structure anymore
	freeaddrinfo(server_info);
	
	if (listen(sockfd, BACKLOG) == -1) {
		perror("Error in listen()");
		exit(EXIT_FAILURE);
	}
	
	return sockfd;
}




void setup_signchld_handler() {
	struct sigaction sa;
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sigchld_handler;
	
	if ( sigaction(SIGCHLD, &sa, 0) == -1) {
		perror("Error in sigaction()");
		exit(EXIT_FAILURE);
	}
}




void sigchld_handler(int sig) {
	int saved_errno = errno;
	pid_t pid;
	int status;
	
	// for waitpid() pid == -1 means any child process
	while ( (pid = waitpid(-1, &status, WNOHANG)) > 0) {
		if (WIFEXITED(status)) {
			printf("--> Child process %d exited with code %d\n",
					pid, WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			printf("--> Child process %d killed with code %d\n",
					pid, WTERMSIG(status));
		}
	}
	errno = saved_errno;
}
