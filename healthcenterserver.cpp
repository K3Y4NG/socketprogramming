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

#define PORT "21968"  // hard coded port of health server
#define BACKLOG 1	 // how many pending connections queue will hold
#define MAXDATASIZE 500 // maximum size of data transfer

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// Function to get the IP address from sockaddr. The code snippet has been taken from beej's.
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void *get_sockaddr_in_addr(struct in_addr *sa)
{
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// The main fucntion of the program. All the send and receive is done here.
int main(void)
{
	///////////////////////////////////// Reading users.txt /////////////////////////////////////////
	int USERS=2;
	int BUFFSIZE=20; 
	FILE *fp;
	char *lines[USERS][BUFFSIZE];
	fp=fopen("users.txt","r");
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

	// token_names: contain the names of the patients
	// taken_pass: contains the corresponding password of the patients
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
	// Finished with one patient.
	// Next patient. Patient2.
	token = strtok(lines[1][BUFFSIZE], " ");
	token_name[1][BUFFSIZE] = strdup(token);
	token=strtok(NULL," ");
	token = strtok(token, "\n");
	token_pass[1][BUFFSIZE] = strdup(token);
	string name2=(string)token_name[1][BUFFSIZE];
	string pass2=(string)token_pass[1][BUFFSIZE];
	///////////////////////////////////////////////////// Finished reading user.txt ////////////////////////////////////////////////

	////////////////////////////////////////////////////// Reading availabilities.txt /////////////////////////////////////////////
	USERS=6;
	BUFFSIZE=30; 
	string availabilities[6][5];
	FILE *fp2;
	fp2=fopen("availabilities.txt","r");
	if(fp2 == NULL) {
 	  perror("Error opening file");
   	  return(-1);
 	}
 	fstream f("availabilities.txt", fstream::in );
 	string dummy="";
 	int j;
 	for(int i=0;i<6;i++){
 		getline(f, dummy);
 			j=0;
 			istringstream iss(dummy);
 			do
	   		{
	   			if(j>4){
	   				break;
	   			}
		        iss >> availabilities[i][j];
		        j++;
	    	} while (iss);
	    	//cout<<"\n";
	    	cout.flush();
 	}
	fclose(fp2);
	// //////////////////////////////////////////////////finished reading availabilities.txt ////////////////////////////////////////////////

	int sockfd, new_fd, numbytes;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;
	char buf[MAXDATASIZE];
	char buf1[MAXDATASIZE];
	char buf2[MAXDATASIZE];
	char buf3[MAXDATASIZE];
	/**
	 The maintaind list, whch the server stores to reserve an appointment. 1 for free appointment and 0 for reserved.
	*/
	int maintainedList[6] = {1,1,1,1,1,1};   

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// This code snipped for looping through all the pointers of getaddrinfo has benn taken from beej's guide.
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}
		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
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
	int z;
	struct sockaddr_in adr_inet;
	int len_inet= sizeof adr_inet;
	z = getsockname(sockfd, (struct sockaddr *)&adr_inet, (socklen_t*)&len_inet);  
    if ( z == -1 ) {  
    	perror("getsockname()");
       exit(1); 
    }  
    cout<<"Phase1: The Health Server Center has TCP port number "<<(unsigned)(adr_inet.sin_port)<< " and IP address ";
    for(i1 = 0; addr_list[i1] != NULL; i1++) {
        printf("%s.\n", inet_ntoa(*addr_list[i1]));
    }
    
	freeaddrinfo(servinfo); 

	// Listen to socket.
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	// The main loop, it will exit only when the user exits.
	while(1) {  
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
		
		
		// Receival of authentication command!
		if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
	    	perror("recv");
	    	exit(1);
		}
		buf[numbytes] = '\0';

		// Comparing the received info with the one stored in users.txt.
		
		int loop_var=1;
		bool authenticate=false;
		string name;
		string pass;
		string command;
		istringstream iss(buf);
		do
   		{
	        string sub;
	        iss >> sub;
	        if(loop_var==1){
	        	command=sub;
	        }
	        if(loop_var==2){
	        	name=sub;
	        }
	        if(loop_var==3){
	        	pass=sub;
	        }
	        loop_var=loop_var+1;
    	} while (iss);
    

    	// For the authenticate command!
  		string au="authenticate";
  		string g=name1;
    	if(command.compare(au)==0){
    		if(name.compare(name1)==0){
    			if(pass.compare(pass1)==0){
    				authenticate=true;

    			}
    		}
    		if(name.compare(name2)==0){
    			if(pass.compare(pass2)==0){
    				authenticate=true;
    			}
    		}
    	}

    	if(authenticate==true && name.compare(name1)==0){
    		if (send(new_fd, "sucess", 13, 0) == -1){
				perror("send");
				exit(1);
			}
			cout<<"Phase1: The Heath Center Server has received request from a patient with username "<<name1<<" and password "<<pass1<<"."<<endl;
			cout.flush();
			cout<<"Phase1: The Heath Center Server sends the response sucess to patient with username "<<name1<<"."<<endl;
			cout.flush();
    	}
    	if(authenticate==true && name.compare(name2)==0){
    		if (send(new_fd, "sucess", 13, 0) == -1){
				perror("send");
				exit(1);
			}
			cout<<"Phase1: The Heath Center Server has received request from a patient with username "<<name2<<" and password "<<pass2<<"."<<endl;
			cout.flush();
			cout<<"Phase1: The Heath Center Server sends the response sucess to patient with username "<<name2<<"."<<endl;
			cout.flush();
    	}
    	if(authenticate==false && name.compare(name1)==0){
    		cout<<"Phase1: The Heath Center Server has received request from a patient with username "<<name1<<" and password "<<pass1<<"."<<endl;
			cout.flush();
    		cout<<"Phase1: The Heath Center Server sends the response failure to patient with username "<<name1<<"."<<endl;
			cout.flush();
    		if (send(new_fd, "failure", 13, 0) == -1){
				perror("send");	
				exit(1);
			}
    	}
    	if(authenticate==false && name.compare(name2)==0){
    		cout<<"Phase1: he Heath Center Server has received request from a patient with username "<<name2<<" and password "<<pass2<<"."<<endl;
			cout.flush();
    		cout<<"Phase1: The Heath Center Server sends the response failure to patient with username "<<name2<<"."<<endl;
			cout.flush();
    		if (send(new_fd, "failure", 13, 0) == -1){
				perror("send");	
				exit(1);
			}
    	}

    	//////////////////////////////////////////////// Start of Phase 2  /////////////////////////////////////////
	    // Get IP address and port number of peer.
		socklen_t len;
		struct sockaddr_storage addr;
		char ipstr[INET6_ADDRSTRLEN];
		int port;

		len = sizeof addr;
		getpeername(new_fd, (struct sockaddr*)&addr, &len);

		if (addr.ss_family == AF_INET) {
		    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
		    port = ntohs(s->sin_port);
		    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
		}else { // AF_INET6
  	  	struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
    	port = ntohs(s->sin6_port);
    	inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
		}
		//printf("Peer IP address: %s\n", ipstr);
		//printf("Peer port      : %d\n", port);
		string ipAddress_c=(string) ipstr;
	 	///////////////////////////////////////
	    if ((numbytes = recv(new_fd, buf3, MAXDATASIZE-1, 0)) == -1) {
	    	perror("recv");
	    	exit(1);
		}
		buf3[numbytes] = '\0';
		cout.flush();
		string availabileSlots=(string) buf3;
		//printf("%s\n", buf);
		
		//cout<<availabileSlots;
		//cout.flush();
		// The patient has sent an available message so that now we can send a list of available time slots!
		if(availabileSlots.compare("available")==0){
			cout.flush();
			cout<<"Phase2: The Health Center Server, receives a request for available time slots from patients with port number "<<port<<" and IP address "<<ipAddress_c<<".\n";
			cout.flush();
			// assume there is an array called maintainedList !!! mapping 1 to be reserved and - to be free. It will have 6 entries. One for each slot.
			string str="";
			// Creating a list of available slots!
			for(int i=0;i<6;i++){
				if(maintainedList[i]==1){
					cout.flush();
					//cout<<123;
					cout.flush();
					for(int j=0;j<3;j++){
						str=str+availabilities[i][j];
						if(j!=2){
							str=str+" ";
						}
						//cout<<str;
						//cout.flush();
					}	
					str=str+"\n";
				}
				
			}
			
			//cout<<str;
			//cout.flush();
			char* url = new char[str.length()+1];
			strcpy(url, str.c_str());
			// Sending a list of available slots.

			if(name.compare(name1)==0){
				cout<<"Phase 2: The Health Center Server sends available time slots to patient with username "<<name1<<".\n";
				cout.flush();
			}
			if(name.compare(name2)==0){
				cout<<"Phase 2: The Health Center Server sends available time slots to patient with username "<<name2<<".\n";
				cout.flush();
			}
			if (send(new_fd, url, 500, 0) == -1){
				perror("send");
				exit(1);
			}

			// Recevie the selection
			if ((numbytes = recv(new_fd, buf3, MAXDATASIZE-1, 0)) == -1) {
		    	perror("recv");
		    	exit(1);
			}
			buf3[numbytes] = '\0';
			istringstream iss(buf3);
			loop_var=1;
			string selection="";
			char* index;
			do
   			{
		        string sub;
		        iss >> sub;
		        if(loop_var==1){
		        	selection=sub;
		        }
		        if(loop_var==2){
		        	char* url1 = new char[sub.length()+1];
					strcpy(url1, sub.c_str());
		        	index=url1;
		        }
		        loop_var=loop_var+1;
    		} while (iss || loop_var<=2);
    		char* url2;
    		if(selection.compare("selection")==0){
    			// go over available time slots and see if available time slot is free
    			// mark the slot reserved
    			// check with doctors deatails and send 4th and 5th column.
    			// close TCP connection after patient receives this.
    			// check with doctors deatails and send 4th and 5th column.
    			if(name.compare(name1)==0){
    				cout<<"Phase 2: The Health Center Server receives a request for appointment "<<atoi(index)<<" from patient with port number "<<port<<" and username "<<name1<<".\n";
    			}
    			if(name.compare(name2)==0){
    				cout<<"Phase 2: The Health Center Server receives a request for appointment "<<atoi(index)<<" from patient with port number "<<port<<" and username "<<name2<<".\n";
    			}
    			if(maintainedList[atoi(index)-1]==0){
    				if(name.compare(name1)==0){	
						cout<<"Phase 2: The Health Center Server rejects the following appointment, "<<index<<" to patient with username "<<name1<<".\n";
					}
					if(name.compare(name2)==0){	
						cout<<"Phase 2: The Health Center Server rejects the following appointment, "<<index<<" to patient with username "<<name2<<".\n";
					}

    				if (send(new_fd, "notavailable", 100, 0) == -1){
						perror("send");
						exit(1);
					}
    			}
    			if(maintainedList[atoi(index)-1]==1){
    				maintainedList[atoi(index)-1]=0;
    				string str;
    				for(int j=3;j<=4;j++){
						str=str+availabilities[atoi(index)-1][j];
						if(j!=4){
							str=str+" ";
						}
					}
					//
					//cout<<str<<"\n";
					url2 = new char[str.length()+1];
					strcpy(url2, str.c_str());
					//printf("%s\n", url2);
					url2[str.length()+1]='\0';

					if(name.compare(name1)==0){	
						cout<<"Phase 2: The Health Center Server confirms the following appointment, "<<index<<" to patient with username "<<name1<<".\n";
					}
					if(name.compare(name2)==0){	
						cout<<"Phase 2: The Health Center Server confirms the following appointment, "<<index<<" to patient with username "<<name2<<".\n";
					}
					if (send(new_fd, url2, 100, 0) == -1){
						perror("send");
						exit(1);
					}
    			}
    		}
    		cout.flush();

		}

		


		close(new_fd);
	}

	return 0;
}
