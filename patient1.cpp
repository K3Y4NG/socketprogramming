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
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

#define PORT "21968" // Port of the health server.
#define MAXDATASIZE 500 // max number of bytes we can get at once 

// This code snippet to get the ip address from sockaddr was taken from beej's.
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	///////////////////////////////////////////////// Reading patient1.txt /////////////////////////////////////////
	int USERS=1;
	int BUFFSIZE=20; 
	FILE *fp;
	char *lines[USERS][BUFFSIZE];
	fp=fopen("patient1.txt","r");
	 if(fp == NULL) {
 	  perror("Error opening file");
   	  return(-1);
 	}
	int i=0;

	while (i < USERS)
	{
		char str[BUFFSIZE];
		if(fgets(str, sizeof(lines[i]), fp)!=NULL){
			lines[i][BUFFSIZE]=strdup(str);
		}
		i = i + 1;
	}
	fclose(fp);

	char *token_name[USERS][BUFFSIZE];
	char *token_pass[USERS][BUFFSIZE];
	char *token;
	token = strtok(lines[0][BUFFSIZE], " ");
	token_name[0][BUFFSIZE] = strdup(token);
	token=strtok(NULL," ");
	token = strtok(token, "\n");
	token_pass[0][BUFFSIZE] = strdup(token);
	string name1=(string)token_name[0][BUFFSIZE];
	string pass1=(string)token_pass[0][BUFFSIZE];
	cout.flush();
	cout.flush();
	
	int sockfd, numbytes, numbytess;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *p;
	struct addrinfo *servinfo ;// will point to results
	int rv, getsock_check;
	char s[INET6_ADDRSTRLEN];
	char buf2[MAXDATASIZE];
	if (argc != 1) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_INET; 
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

	if ((rv = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0) { 
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// This loop through the results has been taken from beej's guide.
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
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
	// Code to get port number of patient.
	int z;
	struct sockaddr_in adr_inet;
	int len_inet= sizeof adr_inet;
	z = getsockname(sockfd, (struct sockaddr *)&adr_inet, (socklen_t*)&len_inet);  
    if ( z == -1 ) {  
    	perror("getsockname()");
       exit(1); 
    } 
    cout<<"Phase1: The Patient 1 has TCP port number "<<(unsigned)(adr_inet.sin_port)<< " and IP address " << inet_ntoa(adr_inet.sin_addr)<<endl;
    
    std::stringstream ss; 
	ss << adr_inet.sin_port;
	std::string portNumber1 = ss.str();
    string ipAddresss1 = inet_ntoa(adr_inet.sin_addr);

	char* portNumber = new char[portNumber1.length()+1];
	strcpy(portNumber, portNumber1.c_str());
	char* ipAddresss = new char[ipAddresss1.length()+1];
	strcpy(ipAddresss, ipAddresss1.c_str());

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
	//printf("client: connecting to %s\n", s);

	string to_authenticate="authenticate "+name1+" "+pass1;
	char* url = new char[to_authenticate.length()+1];
	strcpy(url, to_authenticate.c_str());

	// Sending the authentication request.
	if (send(sockfd, url, 100, 0) == -1){
		perror("send");
		exit(1);
	}

	cout<<"Phase1: Authentication request from Patient 1 with username "<<name1<<" and password "<<pass1<<" has been sent to the Health Service Center. \n";
	cout.flush();
	freeaddrinfo(servinfo); // all done with this structure

	// Receivng request, result of the authentication!
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}
	buf[numbytes] = '\0';
	string auth_received=(string) buf;
	cout.flush();
	if(auth_received.compare("sucess")==0){
		cout<<"Phase1: Patient 1 authentication result: sucess. \n";
		cout.flush();
	}
	if(auth_received.compare("failure")==0){
		cout<<"Phase1: Patient 1 authentication result: failure. \n";
		cout<<"Phase1: End of Phase 1 for Patient 1. \n";
		close(sockfd);
		exit(1);
	}
	cout<<"Phase1: End of Phase 1 for Patient1. \n";
	cout.flush();

	////////////////////////////////////////////// Start of Phase 2 //////////////////////////////////////

	if (send(sockfd, "available", 13, 0) == -1){
		perror("send");
		exit(1);
	}

	// Receival of time slots!
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}	
	buf[numbytes] = '\0';
	string recievedLsit=(string) buf;
	cout<<"Phase 2: The following appointments are available for Patient1:\n";
	cout.flush();
	cout<<recievedLsit;
	cout.flush();
	cout<<"Please enter the preferred appointement index and press enter: ";
	cout.flush();
	string index;
	cin>>index;
	string dummy="";
	string num;
	int j=0;
  	char* sentence=buf;
  	bool comparison=false;
 	while (comparison==false)
 	{
 		std::stringstream ss1(recievedLsit);
  		std::string to;
		while(std::getline(ss1,to,'\n')){
      	//cout << to <<endl;
      	j=0;
      	istringstream iss(to);
 			do
	   		{
	   			if(j==0){
	   				iss >> num;
	   				if(num.compare(index)==0){
	   					//cout<<num<<endl;
	   					comparison=true;
	   				}
	   				cout.flush();
	   				j++;
	   				break;

	   			}
	    	} while (iss);
   	 	}
   	 	if(comparison==true){
    		break;
    	}
    	else{
    		cout<<"Phase 2: The appointment index did not match. Please enter again: ";
    		cin>>index;
    		cout.flush();
    	}
 	}

 	// Sends the index to the health center.
 	string selection="selection "+index;
	char* url1 = new char[selection.length()+1];
	strcpy(url1, selection.c_str());

	// Sending the selesction
	if (send(sockfd, url1, 100, 0) == -1){
		perror("send");
		exit(1);
	}
 	i=0;
	do{
	    if ((numbytess = recv(sockfd, buf2, 100, 0)) == -1) {
	    	perror("recv");
	   	 	exit(1);
		}
		i++;
	}while(i<2);
	buf2[numbytess] = '\0';
	string receiveddoc=(string) buf2;
	
	
	// If received notavailable the client closes the TCP connection and exit immediately.
	if(receiveddoc.compare("notavailable")==0){
		cout<<"Phase 2: The requested appointment from Patient1 is not available. Exiting ....";
		close(sockfd);
		exit(1);
	}
	// Patient has received the the port numner. Time to close the TCP connection.
	close(sockfd);
	int loop_var=0;
	string doc_name;
	string port_number_doc;
	istringstream iss(receiveddoc);
		do
   		{
	        string sub;
	        iss >> sub;
	        if(loop_var==0){
	        	doc_name=sub;
	        }
	        if(loop_var==1){
	        	port_number_doc=sub;
        	}
        	loop_var=loop_var+1;
    	} while (iss || loop_var<2) ;

	cout<<"Phase 2: The requested appointment is available and reserved to Patient1. The assigned doctor port number is "<<port_number_doc<<".\n";

	////////////////////////////////////////// Phase 3 ////////////////////////////////////////////
	/////////////////////////////////Read from patient1insurance.txt. ///////////////////

	 USERS=1;
	 BUFFSIZE=30; 
	FILE *fp1;
	char *lines1[USERS][BUFFSIZE];
	fp1=fopen("patient1insurance.txt","r");
	 if(fp1 == NULL) {
 	  perror("Error opening file");
   	  return(-1);
 	}
	 i=0;

	while (i < USERS)
	{
		char str[BUFFSIZE];
		if(fgets(str, sizeof(lines1[i]), fp1)!=NULL){
			lines1[i][BUFFSIZE]=strdup(str);
		}
		i = i + 1;
	}
	fclose(fp1);

	char *token_insurance[USERS][BUFFSIZE];
	char* token1;
	token1 = strtok(lines1[0][BUFFSIZE], "\n");
	token_insurance[0][BUFFSIZE] = strdup(token1);
	string insurance=(string)token_insurance[0][BUFFSIZE];
	
	/////////////////////////////////////////////////////finished reading !////////////////////////////////////////////////
	
	int sockfd1;
	struct addrinfo hints1, *servinfo1, *p1;
	int rv1;
	int numbytes1;

	memset(&hints1, 0, sizeof hints1);
	hints1.ai_family = AF_INET;
	hints1.ai_socktype = SOCK_DGRAM;

	char* url3 = new char[port_number_doc.length()+1];
	strcpy(url3, port_number_doc.c_str());
	if ((rv1 = getaddrinfo("localhost", url3, &hints1, &servinfo1)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv1));
		return 1;
	}

	// loop through all the results and make a socket
	for(p1 = servinfo1; p1 != NULL; p1 = p1->ai_next) {
		if ((sockfd1 = socket(p1->ai_family, p1->ai_socktype, p1->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}

	if (p1 == NULL) {
		fprintf(stderr, "talker: failed to bind socket\n");
		return 2;
	}
	
	struct sockaddr * addreSS=p1->ai_addr;
	//struct sockaddr_in * addreSS2=addreSS;
	size_t size=p1->ai_addrlen;

	if ((numbytes1 = sendto(sockfd1, "hello world!", 13, 0, addreSS, size)) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	/////////////////////////////Get port number and IP add. of ptient1 UDP/////////////////////////////////////////
	int z1;
	struct sockaddr_in adr_inet1;
	int len_inet1= sizeof adr_inet1;
	z1 = getsockname(sockfd1, (struct sockaddr *)&adr_inet1, (socklen_t*)&len_inet1);  
    if ( z1 == -1 ) {  
    	perror("getsockname()");
       exit(1); 
    } 
    cout<<"Phase 3: The Patient 1 has dynamic UDP port number "<<(unsigned)(adr_inet1.sin_port)<< " and IP address " << s<<".\n";
    cout.flush();

	// Time to send the insurance name to the doctor ////////////////////
	char* url11 = new char[insurance.length()+1];
	strcpy(url11, insurance.c_str());
	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[INET6_ADDRSTRLEN];
	int port;

	len = sizeof addr;
	getpeername(sockfd1, (struct sockaddr*)&addr, &len);

	if (addr.ss_family == AF_INET) {
	    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	    port = ntohs(s->sin_port);
	    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
	}else { // AF_INET6
	  	struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
		port = ntohs(s->sin6_port);
		inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
	}
	if ((numbytes1 = sendto(sockfd1, url11, 20, 0, addreSS, size)) == -1) {
		perror("talker: sendto");
		exit(1);
	}
	inet_ntop(p1->ai_family, get_in_addr(addreSS), s, sizeof s);
	cout<<"Phase 3: The cost estimation request from Patient1 with insurance plan "<<insurance<<" has been sent to the doctor with port number "<<port_number_doc<< " and IP address "<<s<<".\n";

	// Receive the amount info!
	int size1=sizeof addreSS;
	if ((numbytes1 = recvfrom(sockfd, buf2, 100 ,0, addreSS, (socklen_t *)&size1)) == -1) {
		perror("recvfrom");
		exit(1);
	}
	cout<<"Phase 3: Patient 1 receives "<<(string)buf2<<"$ estimation cost from docor with port number "<<port_number_doc<< " and IP address "<<s<<".\n";
	cout<<"Phase 3: End of Phase 3 for Patient 1.\n";
	freeaddrinfo(servinfo1);
	close(sockfd1);
	return 0;
}
