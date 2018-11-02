/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

// #define PORT "3490" // the port client will be connecting to, not really tho
#define MAXDATASIZE 100000 // max number of bytes we can get at once
#define MAXINPUTSIZE 200
#define STR400 "HTTP/1.0 400 Bad Request\r\n"
#define STR404 "HTTP/1.1 404 Not Found\r\n"
#define STR200 "HTTP/1.0 200 OK\r\n"
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];


	if (argc != 2) {
	    fprintf(stderr,"usage: ./http_client http:/hostname[:port]/path/to/file\n");
	    exit(1);
	}


	//parse the input
	// int usage_flag=0;
	char port[10]; //by default
	char file_path[MAXINPUTSIZE];
	char hostname[MAXINPUTSIZE];
	char input[MAXINPUTSIZE];
	int port_position=0;
	int port_flag=0;
	int file_path_position=0;
	strcpy(input,argv[1]);
	int j, k;
	if(strncmp(input,"http://",7)!=0){
		fprintf(stderr,"usage: ./http_client http:/hostname[:port]/path/to/file\n");
		exit(1);
	}
	for(int i = 7;i<strlen(input);i++){
		if((char)input[i]==':'){
			port_position=i;
			port_flag=1;
		}
		if((char)input[i]=='/'){
			file_path_position=i;
			break;
		}
	}
	if(port_flag){
		for(int i=7;i<port_position;i++){
			hostname[i-7]=input[i];
			}
			hostname[port_position-7]='\0';
		j = 0;
		for(int i=port_position+1;i<file_path_position;i++){
			port[j]=input[i];
			j++;
			}
			port[j]='\0';
		k = 0;
		for(int i=file_path_position+1;i<strlen(input);i++){
			file_path[k]=input[i];
			k++;
			}
			file_path[k]='\0';
		}
	else{
		strcpy(port,"80");
		j=0;
		for(int i =7; i <file_path_position;i++){
			hostname[j]=input[i];
			j++;
		}
		hostname[j]='\0';
		k=0;
		for(int i = file_path_position;i<strlen(input);i++){
			file_path[k]=input[i];
			k++;
		}
		file_path[k]='\0';
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);



	freeaddrinfo(servinfo); // all done with this structure

	//encoding request
	char request[MAXDATASIZE];
	memset(request, '\0', sizeof request);
	sprintf(request, "GET /%s HTTP/1.0\r\n", file_path);

	//send the request
	if(numbytes= send(sockfd,request,strlen(request),0)==-1){
		perror("client:sending request failed");
	}

	//creat a output file
	FILE* output;
	if((output=fopen("output","wb"))==NULL){
		perror("client:creating output file failed");
		exit(1);
	}

	//write received buf to output
	while((numbytes = recv(sockfd, buf, MAXDATASIZE, 0))>0){
		
		// if( numbytes <0) {
		//     perror("recv");
		//     exit(1);
		// }
		// else{
			fwrite(buf,numbytes,1,output);
		// }
		// if(numbytes==0)
		// else if(numbytes != 0){
		// 	if(strncmp(buf,STR400,strlen(STR400)) == 0 || strncmp(buf,STR404,strlen(STR404)) == 0)
		// 	{
		// 		break;
		// 	}
		// 	else if(strncmp(buf,STR200,strlen(STR200)) == 0)
		// 	{
		// 		fwrite(buf, numbytes, 1, output);
		// 	}
		// }
		// else{
		// 	break;
		// }
	}

	printf("%s\n", buf);

	fclose(output);
	close(sockfd);

	return 0;
}

