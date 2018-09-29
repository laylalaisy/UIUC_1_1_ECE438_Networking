#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define BACKLOG 20	 		// how many pending connections queue will hold
#define REQUEST_LEN 2048	// length of request from client
#define FILENAME_LEN 256	// length of filenmae from request

#define STR400 "HTTP/1.0 400 Bad Request\r\n"
#define STR404 "HTTP/1.1 404 Not Found\r\n"
#define STR200 "HTTP/1.0 200 OK\r\n"

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);	// IPv4
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);	// IPv6
}

void handle_request(int new_fd)
{
	char request_buf[REQUEST_LEN];
	int request_len;
	char filename[FILENAME_LEN];
	FILE* file;
	char ch;

	request_len = read(new_fd, request_buf, REQUEST_LEN);
	// fail to receive request
	if(request_len == 0 || request_len == -1 || request_len == REQUEST_LEN)
	{
		fprintf(stderr, "fail to receive request from client\n");
		if(write(new_fd, (char*)STR400, strlen(STR400)) == -1)
		{
			perror("write");
		}
		exit(1);
	}

	// received request from client and handle the request
	for(int i = 0; i < request_len; i++)
	{
		if(request_buf[i] == '\r' && request_buf[i+1] == '\n')
		{
			request_buf[i] = '\0';
		}
	}
	request_buf[request_len] = '\0';

	// check the request: GET
	if(strncmp(request_buf, "GET ", 4) != 0)
	{
		fprintf(stderr, "fail to receive request from client\n");
		if(write(new_fd, (char*)STR400, strlen(STR400)) == -1)
		{
			perror("write");
		}
		exit(1);
	}

	// read request's file name
	int j = 0;
	for(int i = 5; request_buf[i] != ' '; i++)
	{
		filename[j] = request_buf[i];
		j++;
	}
	filename[j] = '\0';
	printf("filename: %s\n", filename);

	// open and send file
	file = fopen(filename, "r");
	// fail to open file
	if(!file)
	{
		fprintf(stderr, "fail to find request file from client\n");
		if(write(new_fd, (char*)STR404, strlen(STR404)) == -1)
		{
			perror("write");
		}
		exit(1);
	}
	else
	{
		// send ok response
		if(write(new_fd, (char*)STR200, strlen(STR200)) == -1)
		{
			perror("write");
		}
		// send file
		while((ch = fgetc(file)) && (ch != EOF))
		{
			write(new_fd, &ch, 1);
		}
	}
	close(new_fd);
}


int main(int argc, char* argv[])
{
	struct addrinfo hints;
	struct addrinfo* servinfo;
	struct addrinfo* p;
	int rv;
	int sockfd;
	int new_fd;
	int yes = 1;
	struct sigaction sa;
	struct sockaddr_storage their_addr;	// connector's address information
	socklen_t sin_size;
	pid_t child_pid;
	char s[INET6_ADDRSTRLEN];

	// get port
	char* port = strdup(argv[1]);

	// check input: './http_server <port>'
	if(argc != 2)
	{
		fprintf(stderr, "input: ./http_server <port>\n");
		exit(1);
	}

	// bind the socket
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;	// IPv4 & IPv6
	hints.ai_socktype = SOCK_STREAM;// TCP
	hints.ai_flags = AI_PASSIVE; 	// use my IP
	
	// getaddrinfo 
	if((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}
		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			perror("setsockopt");
			exit(1);
		}
		if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	// fail on bind
	if(p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	// all done with this structure and use sockfd later
	freeaddrinfo(servinfo);	

	// pending queue
	if(listen(sockfd, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}

	// reap all dead processes
	sa.sa_handler = sigchld_handler; 
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	// waiting for connection
	printf("server: waiting for connections...\n");

	// main accept() loop
	while(1)
	{
		sin_size = sizeof(their_addr);

		// accept
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

		// fail to accept
		if(new_fd == -1)
		{
			perror("accept");
			continue;
		}

		// fork
		child_pid = fork();
		if(child_pid == -1)		// fail to fork
		{
			perror("fork");
		}
		else if(child_pid == 0)	// child 
		{
			// change ip address
			inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
			printf("server: got connection from %s\n", s);

			// handle request
			handle_request(new_fd);
		}

		// father
		close(new_fd);
	}

	return 0;
}
