#include <stdio.h> 
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <netdb.h>
	
#define TRUE 1 
#define FALSE 0 
#define PORT 1234 
char myhostname[1024];
#define SIZE 1024

//void send_file(int sockrec, int sockdes){
//  int n;
////  FILE *fp;
////  char *filename = "recv.txt";
//  char buffer[1024];
//
////  fp = fopen(filename, "w");
//  while (1) {
//    n = recv(sockrec, buffer, SIZE, 0);
//    if (n <= 0){
//      break;
//      return;
//    }
//    if (send(sockdes, buffer, sizeof(buffer), 0) == -1) {
//      perror("[-]Error in sending file.");
//      exit(1);
//    }
//    bzero(buffer, SIZE);
//  }
//}
                        


int main(int argc , char *argv[]) 
{ 
	int opt = TRUE; 
	int master_socket , addrlen , new_socket , client_socket[30] , 
		max_clients = 30 , activity, i , valread , sd; 
	int max_sd; 
    char user_list[100][50];
    
    memset(user_list, 0, sizeof user_list);
    
    
	struct sockaddr_in address; 
    struct hostent *heLocalHost;
		
	char buffer[1025]; //data buffer of 1K 
		
	//set of socket descriptors 
	fd_set readfds; 
		
	//a message 
	char *message = "ECHO Daemon v1.0 \r\n"; 
	
	//initialise all client_socket[] to 0 so not checked 
	for (i = 0; i < max_clients; i++) 
	{ 
		client_socket[i] = 0; 
	} 
		
	//create a master socket 
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	//set master socket to allow multiple connections , 
	//this is just a good habit, it will work without this 
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
		sizeof(opt)) < 0 ) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	
	//type of socket created 
//    gethostname(myhostname, 1023);
//    heLocalHost = gethostbyname(myhostname);
	address.sin_family = AF_INET; 
//    address.sin_addr = *(struct in_addr*) heLocalHost->h_addr;
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
//    heLocalHost->h_addr;
//    memset(&(address.sin_zero),0,8);
		
	//bind the socket to localhost port 8888 
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	printf("Listener on port %d \n", PORT); 
		
	//try to specify maximum of 3 pending connections for the master socket 
	if (listen(master_socket, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
		
	//accept the incoming connection 
	addrlen = sizeof(address); 
	puts("Waiting for connections ..."); 

	while(TRUE) 
	{ 
		//clear the socket set 
		FD_ZERO(&readfds); 
	
		//add master socket to set 
		FD_SET(master_socket, &readfds); 
		max_sd = master_socket; 
			
		//add child sockets to set 
		for ( i = 0 ; i < max_clients ; i++) 
		{ 
			//socket descriptor 
			sd = client_socket[i]; 
				
			//if valid socket descriptor then add to read list 
			if(sd > 0) 
				FD_SET( sd , &readfds); 
				
			//highest file descriptor number, need it for the select function 
			if(sd > max_sd) 
				max_sd = sd; 
		} 
	
		//wait for an activity on one of the sockets , timeout is NULL , 
		//so wait indefinitely 
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); 
	
		if ((activity < 0) && (errno!=EINTR)) 
		{ 
			printf("select error"); 
		} 
			
		//If something happened on the master socket , 
		//then its an incoming connection 
		if (FD_ISSET(master_socket, &readfds)) 
		{ 
			if ((new_socket = accept(master_socket, 
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
			{ 
				perror("accept"); 
				exit(EXIT_FAILURE); 
			} 
			
			//inform user of socket number - used in send and receive commands 
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs 
				(address.sin_port)); 
		
			//send new connection greeting message 
//			if( send(new_socket, message, strlen(message), 0) != strlen(message) ) 
//			{ 
//				perror("send"); 
//			} 
				
			puts("Welcome message sent successfully"); 
				
			//add new socket to array of sockets 
			for (i = 0; i < max_clients; i++) 
			{ 
				//if position is empty 
				if( client_socket[i] == 0 ) 
				{ 
                    client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n" , i); 
					break; 
				} 
			} 
		} 
			
		//else its some IO operation on some other socket 
		for (i = 0; i < max_clients; i++) 
		{ 
			sd = client_socket[i]; 
            int g;
				
			if (FD_ISSET( sd , &readfds)) 
			{ 
				//Check if it was for closing , and also read the 
				//incoming message 
				if ((valread = read( sd , buffer, 1024)) == 0) 
				{ 
					//Somebody disconnected , get his details and print 
					getpeername(sd , (struct sockaddr*)&address ,
						(socklen_t*)&addrlen); 
					printf("Host disconnected , ip %s , port %d \n" , 
						inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
                    
//                    char message[100];
//                    strcpy(message, "{'Message':'Disconnect','id':'");
//                    char kic[10];
//                    sprintf(kic, "%d", i);
//                    strcat(message, kic);
//                    strcat(message, "'}");
//                    send(client_socket[0], message, strlen(message), 0);
						
					//Close the socket and mark as 0 in list for reuse 
//                    user_list[i] = '\0';
                    memset(user_list[i], 0, sizeof user_list[i]);
					close( sd ); 
					client_socket[i] = 0; 
				} 
					
				//Echo back the message that came in 
				else { 
                    char* name;
                    char* dest;
                    char* msg;
                    char* fileSize;
                    long long int sizeOfFile;
//                    char* msg2;
//                    puts("a");
//                    puts("%s", buffer);
                    char delim[] = "$"; 
                    buffer[valread] = '\0';
//                    send(sd , buffer , strlen(buffer) , 0 ); 
                    char *ptr = strtok(buffer, delim);
                    
//                    printf("%s\n", ptr);
                    name = ptr;
                    msg = ptr;
                    
                    printf("Message from: %s\n", name);
                    
                    ptr = strtok(NULL, delim);
                    
                    dest = ptr;
                    
//                    printf("Wiadomość do: %s\n", dest);
                    
                    if(strcmp(dest,"Welcome") == 0) {
                        strcpy(user_list[i], name);
//                        send(sd , buffer , strlen(buffer) , 0 ); 
                    } else if(strcmp(dest,"List") == 0) {
                        strcpy(msg, "List$");
                        for(g = 0; g < 99; g++) {
                            if(strcmp(user_list[g], "") && g != i) {
                                strcat(msg, user_list[g]);
                                strcat(msg, "$");
                            }
                        }
                        msg[strlen(msg)-1] = '\0';
                        send(sd, msg, strlen(msg), 0 );
                    } else if(strcmp(dest,"File") == 0){
                        ptr = strtok(NULL, delim);
                        msg = ptr;
                        printf("Receiving file: %s\n", msg);
                        ptr = strtok(NULL, delim);
                        fileSize = ptr;
//                        printf("rozmiar pliku: %s\n", fileSize);
                        sizeOfFile = atoi(fileSize);
//                        printf("rozmiar pliku w int: %lld\n", sizeOfFile);
                        FILE *fp;
                        int n = 0;
                        char *filename = msg;
                        char bufferf[SIZE+1];
                        memset(bufferf, 0, sizeof bufferf);
//                        puts("a");
                        
                        fp = fopen(filename, "w");
                        while(sizeOfFile > 0) {
//                            printf("czekamy na tresc pliku\n");
                            n = recv(sd, bufferf, SIZE, 0);
                            if (n <= 0){
                                break;
                            }
//                            printf("wpisujemy do pliku to co dostalismy: %s\n", bufferf);
//                            fprintf(fp, "%s", bufferf);
                            if (fwrite(bufferf, sizeof(char), n, fp) != n)
                            {
                                perror("Write File Error");
                                exit(1);
                            }
//                            fputs(bufferf, fp);
//                            printf("zerujemy buffor\n");
                            bzero(bufferf, SIZE);
//                            printf("zmniejszamy rozmiar\n");
//                            printf("zmniejszamy rozmiar\n");
//                            printf("Ile zostało do odebrania przed odjeciem: %lld \n", sizeOfFile);
                            sizeOfFile = sizeOfFile - n;
//                            printf("Ile odebralismy teraz? %d \n", n);
//                            printf("Ile zostało do odebrania? %lld \n", sizeOfFile);
                        }
                        fclose(fp);
                        printf("File created :) \n");
                    } else {
                        for(g = 0; g < 99; g++) {
//                            printf("Co porownujemy %s", user_list[g]);
//                            printf(" %s", dest);
                            if(strcmp(user_list[g],name) == 0){
//                                printf("%i", g);
                                break;
                            }
                        }
//                        char * msg2 = (char *) malloc(1 + strlen(name)+ strlen(msg) );
//                        puts("a");
                        strcat(name, "$");
//                        puts("a");
                        ptr = strtok(NULL, delim);
                        msg = ptr;
                        char * msg2 = (char *) malloc(1 + strlen(name)+ strlen(msg) );
                        strcpy(msg2, name);
                        strcat(msg2, msg);
//                        printf("Message : %s\n", msg2);
//                        buffer[valread] = '\0'; 
                        msg2[strlen(msg2)] = '\0';
                        for(int v = 0; v < 99; v++) {
                           if(v != g && strlen(user_list[v])>1) {
//                           printf("gdzie wysylamy %d\n", v);
                           sd = client_socket[v]; 
					       send(sd , msg2 , strlen(msg2) , 0 ); 
                           }
                        }
                    }
//                    buffer[0] = '\0';
//                    msg[0] = '\0';
//                    name[0] = '\0';
//                    dest[0] = '\0';
				} 
			} 
		} 
	} 
		
	return 0; 
} 
