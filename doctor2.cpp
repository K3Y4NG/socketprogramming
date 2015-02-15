#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

#define MYPORT "42968"	// the port patients would be connecting to.

#define MAXBUFLEN 500

// This code snippet to get the ip address from sockaddr was taken from beej’s.
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
in_port_t get_in_port(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return (((struct sockaddr_in*)sa)->sin_port);
    }

    return (((struct sockaddr_in6*)sa)->sin6_port);
}
int main(void)
{
	///////////////////////////////////////////Loading up the insurance file!///////////////////////////////////////////////
	int USERS=3;
	int BUFFSIZE=30; 
	FILE *fp1;
	char *lines[USERS][BUFFSIZE];
	fp1=fopen("doc2.txt","r");
	 if(fp1 == NULL) {
 	  perror("Error opening file");
   	  return(-1);
 	}
	int i=0;

	while (i < USERS)
	{
		char str[BUFFSIZE];
		if(fgets(str, sizeof(lines[i]), fp1)!=NULL){
			lines[i][BUFFSIZE]=strdup(str);
		}
		i = i + 1;
	}
	fclose(fp1);
	char *token_1c[USERS][BUFFSIZE];
	char *token_2c[USERS][BUFFSIZE];
	char *token;
	token = strtok(lines[0][BUFFSIZE], " ");
	token_1c[0][BUFFSIZE] = strdup(token);
	token=strtok(NULL," ");
	token = strtok(token, "\n");
	token_2c[0][BUFFSIZE] = strdup(token);
	string insu1=(string)token_1c[0][BUFFSIZE];
	string cost1=(string)token_2c[0][BUFFSIZE];
	// Finished with one patient.
	// Next patient. Patient2.
	token = strtok(lines[1][BUFFSIZE], " ");
	token_1c[1][BUFFSIZE] = strdup(token);
	token=strtok(NULL," ");
	token = strtok(token, "\n");
	token_2c[1][BUFFSIZE] = strdup(token);
	string insu2=(string)token_1c[1][BUFFSIZE];
	string cost2=(string)token_2c[1][BUFFSIZE];
	// Finished with one insurance.
	// Next patient. Patient2.
	token = strtok(lines[2][BUFFSIZE], " ");
	token_1c[2][BUFFSIZE] = strdup(token);
	token=strtok(NULL," ");
	token = strtok(token, "\n");
	token_2c[2][BUFFSIZE] = strdup(token);
	string insu3=(string)token_1c[2][BUFFSIZE];
	string cost3=(string)token_2c[2][BUFFSIZE];
	//////////////////////////////////////////////////////////////////// End! /////////////////////////////////////
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	int numbytes1;
	struct sockaddr_storage their_addr;
	struct sockaddr_storage their_addr1;
	char buf[MAXBUFLEN];
	char buf1[MAXBUFLEN];
	socklen_t addr_len;
	socklen_t addr_len1;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; 
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; 

	if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	// This loop through the results has been taken from beej's guide.
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}
	/////////////////////////////Get port number and IP add. of ptient1 UDP/////////////////////////////////////////
	int z1;
	struct sockaddr_in adr_inet1;
	int len_inet1= sizeof adr_inet1;
	z1 = getsockname(sockfd, (struct sockaddr *)&adr_inet1, (socklen_t*)&len_inet1);  
    if ( z1 == -1 ) {  
    	perror("getsockname()");
       exit(1); 
    }
     ////////////////////////////////////////////////// Get Host Name ///////////////////////////////////////////////////

    int i1;
    struct hostent *he;
    struct in_addr **addr_list;
    if ((he = gethostbyname("localhost")) == NULL) {  // get the host info
        herror("gethostbyname");
        return 2;
    }
    addr_list = (struct in_addr **)he->h_addr_list;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cout<<"Phase 3: Doctor 2 has static UDP port "<<(unsigned)(adr_inet1.sin_port)<< " and IP address ";
    cout.flush();
    for(i1 = 0; addr_list[i1] != NULL; i1++) {
        printf("%s.\n", inet_ntoa(*addr_list[i1]));
    }
    while(1){
		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, buf, sizeof(buf) , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) { 
			perror("recvfrom");
			exit(1);
		}
		struct sockaddr* ADDD = (struct sockaddr *)&their_addr;
		struct sockaddr* ADDD3 = (struct sockaddr *)&their_addr;
		struct sockaddr* ADDD4 = (struct sockaddr *)&their_addr;
		buf[numbytes] = '\0';
		if ((numbytes1 = recvfrom(sockfd, buf1, 100 ,0, (struct sockaddr *)&their_addr1, &addr_len1)) == -1) {
			perror("recvfrom");
			exit(1);
		}
		buf1[numbytes1] = '\0';
		struct sockaddr* ADDD2 = (struct sockaddr *)&their_addr1;
		socklen_t len;
		struct sockaddr_storage addr;
		char ipstr[INET6_ADDRSTRLEN];
		int port;
		len = sizeof addr;
		getpeername(sockfd, (struct sockaddr*)&addr, &len);
		struct sockaddr* addr_sendto=(struct sockaddr*)&addr;
		size_t size_sendto= sizeof((struct sockaddr*)&addr);

		if (addr.ss_family == AF_INET) {
		    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
		    port = ntohs(s->sin_port);
		    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
		}else { // AF_INET6
		  	struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
			port = ntohs(s->sin6_port);
			inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
		}
		int port_patient=ntohs(get_in_port(ADDD2));
		cout<<"Phase 3: Doctor 2 receives the request from the patient with port number "<<port_patient<<" and the insurance plan "<<(string)buf1<<".\n";
		//////////////// Need to send info now!
		string ins=(string) buf1;
		if(ins.compare(insu1)==0){
			cout.flush();
			if ((numbytes1 = sendto(sockfd, token_2c[0][BUFFSIZE], 20, 0, ADDD, addr_len1)) == -1) {
				cout.flush();
				perror("talker: sendto");
				exit(1);
			}
			cout<<"Phase 3: Doctor 2 has sent estimated price "<<(string)token_2c[0][BUFFSIZE]<<"$ to patient with port number "<<port_patient<<".\n";

		}
		if(ins.compare(insu2)==0){
			cout.flush();
			if ((numbytes1 = sendto(sockfd, token_2c[1][BUFFSIZE], 20, 0, ADDD3, addr_len1)) == -1) {
				cout.flush();
				perror("talker: sendto");
				exit(1);
			}
			cout<<"Phase 3: Doctor 2 has sent estimated price "<<(string)token_2c[1][BUFFSIZE]<<"$ to patient with port number "<<port_patient<<".\n";
		}
		if(ins.compare(insu3)==0){
			cout.flush();
			if ((numbytes1 = sendto(sockfd, token_2c[2][BUFFSIZE], 20, 0, ADDD4, addr_len1)) == -1) {
				cout.flush();
				perror("talker: sendto");
				exit(1);
			}
			cout<<"Phase 3: Doctor 2 has sent estimated price "<<(string)token_2c[2][BUFFSIZE]<<"$ to patient with port number "<<port_patient<<".\n";
		}
	}
	freeaddrinfo(servinfo);
	close(sockfd);
	return 0;
}
