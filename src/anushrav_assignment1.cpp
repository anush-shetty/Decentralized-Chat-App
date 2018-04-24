/*
 * @anushrav_assignment1
 * @author  Anush Shetty <anushrav@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <net/if.h>     //required for ifaddrs
#include <ifaddrs.h>

//for client
#include <cstring>
#include <ctype.h>

//for server
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <iostream>




#include "../include/global.h"
#include "../include/logger.h"

//for server
#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define BUFFER_SIZE 256

//for client
#define TRUE 1
#define MSG_SIZE 256

using namespace std;

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
struct addr{
    string IP;
    string hostname;
    int fd_id;      //id of fdaccept(unique to every client)
    int c_port;
    int msg_sent;
    int msg_recv;
    bool status;
};

void split(char* s, const char* c,vector<char*>& v);
int connect_to_host(char *server_ip, int server_port);
int sendall(int s, char *buf, int *len);
string bufferToString(char* buffer);
char* showIP();
int getClientPort();
void displayException(const char* command_str);
/*
void split(const string& s, char delim,vector<string>& v) {
    int i = 0;
    int pos = s.find(delim);
    if(pos == string::npos){
      v.push_back(s);
      return;
    }
    cout<<s<<" string length "<<s.length()<<"pos is : "<<pos<<"delimiter length "<<s.find(delim)<<endl;
    
    while (pos != string::npos) {
      v.push_back(s.substr(i, pos-i));
      i = pos++;
      pos = s.find(delim, pos);
      cout<< "pos now is : "<<pos <<" string npos is : "<<string::npos<<endl;
      if (pos == string::npos)
         v.push_back(s.substr(i, s.length()));
    }
}*/

int main(int argc, char **argv)
{
  /*Init. Logger*/
  cse4589_init_log(argv[2]);

  /* Clear LOGFILE*/
  fclose(fopen(LOGFILE, "w"));

  /*Start Here*/
  /*  vector<string>  v;
    string input;
    getline(cin,input);
    split(input,' ',v);
    cout<<v.size()<<endl;
    for (int i = 0; i < v.size(); ++i) {
      cout << v[i] << '\n';
   }*/

    //scanf("%d %d %d", &var1, &var2, &var3);
    /*switch(toupper(var1)
      {
      case 'AUTHOR':
      cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "Anush Shetty"); 
      break;
    }
  */
    vector<char*> command; 
    vector<addr> clients(4);
    int v_size;  
    const char* option;
    const char* buf;
    vector<char*> list;
    option = argv[1];
    printf("value of argv[1] is %s\n",argv[1]);
    char *msg_port = (char*) malloc(strlen(argv[2]));
    memset(msg_port, '\0', strlen(argv[2]));
    msg_port = argv[2];
    int nClient = 0;

    if(*option == 's')
    {
      if(argc != 3) 
        {
        printf("Usage:%s [port]\n", argv[0]);
        exit(-1);
      }
       int port, server_socket, head_socket, selret, sock_index, fdaccept=0;
      socklen_t caddr_len,cl_addr_len;
      struct sockaddr_in server_addr, client_addr;
        struct sockaddr_storage cl_addr;
        fd_set master_list, watch_list;

      /* Socket */
      server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(server_socket < 0)
        perror("Cannot create socket");

      /* Fill up sockaddr_in struct */
      port = atoi(argv[2]);
      bzero(&server_addr, sizeof(server_addr));

        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(port);
        char ipstr[INET6_ADDRSTRLEN];
        char host[1024];
        char service[20];
        char *client_ip;
        int client_port;
        

        /* Bind */
        if(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 )
          perror("Bind failed");

        /* Listen */
        if(listen(server_socket, BACKLOG) < 0)
          perror("Unable to listen on port");

        /* ---------------------------------------------------------------------------- */
        printf("Server is connected");
        /* Zero select FD sets */
        FD_ZERO(&master_list);
        FD_ZERO(&watch_list);
        
        /* Register the listening socket */
        FD_SET(server_socket, &master_list);
        /* Register STDIN */
        FD_SET(STDIN, &master_list);

        head_socket = server_socket;

        while(TRUE){
            memcpy(&watch_list, &master_list, sizeof(master_list));

            //printf("\n[PA1-Server@CSE489/589]$ ");
        //fflush(stdout);

            /* select() system call. This will BLOCK */
            selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
            if(selret < 0)
                perror("select failed.");

            /* Check if we have sockets/STDIN to process */
            if(selret > 0){
                /* Loop through socket descriptors to check which ones are ready */
                      for(sock_index=0; sock_index<=head_socket; sock_index+=1){

                          if(FD_ISSET(sock_index, &watch_list)){

                          /* Check if new command on STDIN */
                          if (sock_index == STDIN){
                            char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);

                          memset(cmd, '\0', CMD_SIZE);
                          if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
                            exit(-1);

                          printf("\nI got: %s\n", cmd);
                          /*split(cmd,' ',command);
                          for(int i=0; i<command.size();++i)
                          {
                          printf("command [%s] is %s : ",i,command[i]);
                          }*/
                          split(cmd," ",command);
                          //Process PA1 commands here ...
                          if(strncmp(command[0],"AUTHOR",6) == 0) 
                                  { 
                                    cse4589_print_and_log("[%s:SUCCESS]\n", strtok(command[0],"\n"));
                                    cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "anushrav"); 
                                    cse4589_print_and_log("[%s:END]\n",strtok(command[0],"\n"));

                                  }
                          if(strncmp(command[0],"IP",2) == 0)   
                                  {
                                    cse4589_print_and_log("[%s:SUCCESS]\n", strtok(command[0],"\n"));
                                    cse4589_print_and_log("IP:%s\n",showIP()); 
                                    cse4589_print_and_log("[%s:END]\n",command[0]);
                                  }     
                          if(strncmp(command[0],"PORT",4) == 0)   
                                  {
                                    cse4589_print_and_log("[%s:SUCCESS]\n", strtok(command[0],"\n"));
                                    cse4589_print_and_log("PORT:%s\n",argv[2]); 
                                    cse4589_print_and_log("[%s:END]\n",command[0]);
                                  }        

                          command.clear();        

                          free(cmd);
                        }
                        /* Check if new client is requesting connection */
                        else if(sock_index == server_socket){
                            caddr_len = sizeof(client_addr);
                            cl_addr_len = sizeof(cl_addr);
                            fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, &caddr_len);
                            if(fdaccept < 0)
                                perror("Accept failed.");
                            
                            printf("fdaccept value is %d\n",fdaccept);
                            fflush(stdin);
                            //for obtaining IP and port of client : LIST
                            struct sockaddr_in *addr_in = (struct sockaddr_in *)&client_addr;
                            //client_ip = inet_ntoa(addr_in->sin_addr);

                            inet_ntop(AF_INET,&(addr_in->sin_addr),ipstr,sizeof ipstr);
                            getnameinfo((struct sockaddr *)&client_addr, (socklen_t)(sizeof client_addr), host, sizeof host, service, sizeof service, 0);
                            //client_port = ntohs(*addr_in->sin_port);
                            //getpeername(sock_index,(struct sockaddr *)&client_addr,&caddr_len);
                            //struct sockaddr_in *s = (struct sockaddr_in *)&client_addr;
                            //client_port = ntohs(addr_in->sin_port);
                            //clients.at(0).msg_sent = 234;
                            clients.at(nClient).IP = ipstr;
                            clients.at(nClient).hostname = host;
                            clients.at(nClient).fd_id = fdaccept;
                            clients.at(nClient).status = TRUE;
                            printf("Host: %s\n",host);

                            /* Add to watched socket list */
                            FD_SET(fdaccept, &master_list);
                            if(fdaccept > head_socket) head_socket = fdaccept;


                        }
                        /* Read from existing clients */
                        else{
                            /* Initialize buffer to receieve response */
                            char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                            memset(buffer, '\0', BUFFER_SIZE);

                            if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0){
                                close(sock_index);
                                printf("Remote Host %s terminated connection!\n",ipstr);
                                /* Remove from watched list */
                                FD_CLR(sock_index, &master_list);
                            }
                            else {
                              //Process incoming data from existing clients here ...

                              printf("\nClient sent me: %s\n", buffer);
                              split(buffer," ",command);
                              if(strncmp(command[0],"LOGIN",5) == 0) 
                                  {
                                    printf("inside %s",command[0]);
                                    clients.at(nClient).c_port = atoi(command[0]);
                                    nClient++;
                                  }
                              for(int i=0; i<command.size();++i)
                              {
                                  printf("command [%d] is %s : ",i,command[i]);
                              }
                              command.clear();  //clearing the vector for every new command
                              printf("ECHOing it back to the remote host ... ");
                              if(send(fdaccept, buffer, strlen(buffer), 0) == strlen(buffer))
                                printf("Done!\n");
                                fflush(stdout);
                            }

                            free(buffer);
                        }
                    }
                }
            }
         }
    }
    if(*option == 'c')
    {

        printf("client started");
        

            if(argc != 3) {
                printf("Usage:%s [port]\n", argv[0]);
                exit(-1);
            }
                
                int server;
                //server = connect_to_host(argv[2], atoi(argv[3]));

            while(TRUE){
                printf("\n[PA1-Client@CSE489/589]$ ");
                fflush(stdout);

                char *msg = (char*) malloc(sizeof(char)*MSG_SIZE);
                memset(msg, '\0', MSG_SIZE);
                if(fgets(msg, MSG_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to msg
                    exit(-1);
                split(msg," ",command);
                printf("Message sent will be : %s\n",msg);   
                for(int i=0;i<command.size();i++)
                  printf("command[%d] : %s",i,command.at(i));
                
                //const char *message = msg;                // for getting message for further comparisons (was in line  if(strncmp(message,"LOGIN",5) == 0))

                 if(strncmp(command[0],"AUTHOR",6) == 0) 
                                  { 
                                    cse4589_print_and_log("[%s:SUCCESS]\n", strtok(command[0],"\n"));
                                    cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "anushrav"); 
                                    cse4589_print_and_log("[%s:END]\n",command[0]);

                                  }
                if(strncmp(command[0],"IP",2) == 0)   
                                  {
                                    cse4589_print_and_log("[%s:SUCCESS]\n", strtok(command[0],"\n"));
                                    cse4589_print_and_log("IP:%s\n",showIP()); 
                                    cse4589_print_and_log("[%s:END]\n",command[0]);
                                  } 
                if(strncmp(command[0],"PORT",4) == 0)   
                                  {
                                    cse4589_print_and_log("[%s:SUCCESS]\n", strtok(command[0],"\n"));
                                    cse4589_print_and_log("PORT:%s\n",argv[2]); 
                                    cse4589_print_and_log("[%s:END]\n",command[0]);
                                  }                                                         
                //printf("Comparison of strings : %s\n",strcmp(msg,"LOGIN"));
                if(strncmp(command[0],"LOGIN",5) == 0)
                {   
                  if(command[1] == NULL || command[2] == NULL)
                      {
                        printf("Usage: LOGIN [IP] [PORT]\n");
                        displayException("c");
                        fflush(stdout);
                        
                      }
                  else 
                    {
                        server = connect_to_host(command[1],atoi(command[2]));
                      if(server < 0)
                      {
                        printf("server connection failed: Check if server is running\n");
                        displayException("c");
                        fflush(stdout);
                      }
                      else if(server >= 1)
                      {    //printf("server value is %s \n",server);
                          if(send(server,strcat(command[0],":"),strlen(command[0])+1,0) == strlen(command[0])+1)
                          printf("message sent\n");
                          if(send(server,argv[2],strlen(argv[2]),0) == strlen(msg_port))
                              printf("client port sent \n");
                          //char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                         // memset(buffer, '\0', BUFFER_SIZE);


                          if(recv(server, &v_size, sizeof v_size, 0) >= 0){
                              //emcpy(&nClient,&buffer,sizeof());
                              nClient = ntohl(v_size);
                              printf("Client size is %d \n",nClient);
                              
                          }
                          
                            char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);

                            memset(buffer, '\0', BUFFER_SIZE);
                            /*int received_bytes = 0;
                            int remaining_bytes = BUFFER_SIZE;
                            while (remaining_bytes > 0) {
                              printf("Buffer val is %s size is %d\n",buffer,remaining_bytes);
                              int res =recv(server, buffer+received_bytes, remaining_bytes, 0);
                              if (res < 0) {
                                printf("recv failed");
                                break;
                              }
                              printf("res value is %d\n",res);
                              received_bytes += res;
                              remaining_bytes -=res;
                            }*/
                            if(recv(server, buffer,BUFFER_SIZE,0) > 0)
                            {
                              split(buffer,";",list);

                              printf("Server responded: %s\n", buffer);
                            }
                             int ct = 1;
                            //printf("list 0 %s, list 1 %s , list 2 %s",list[0],list[1],list[2]);
                             for(int i=0;i<nClient*3;i=i+3)
                             {
                              cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", ct,list[i+1], list[i], atoi(list[i+2]));
                              ct++;
                             }
                               fflush(stdout);
                             
                             
                       
                          command.clear();
                          printf("I got: %s(size:%d chars)", msg, strlen(msg));

                          printf("\nSENDing it to the remote server ... ");
                          //if(send(server, msg, strlen(msg), 0) == strlen(msg))
                          //    printf("Done!\n");
                          fflush(stdout);

                          /* Initialize buffer to receieve response */
                         /* char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                          memset(buffer, '\0', BUFFER_SIZE);

                          if(recv(server, buffer, BUFFER_SIZE, 0) >= 0){

                              printf("Server responded: %s", buffer);
                              
                          }
                          memset(buffer, '\0', BUFFER_SIZE);
                          if(recv(server, buffer, BUFFER_SIZE, 0) >= 0)
                          {
                          printf("Server responded: %s", buffer);
                          fflush(stdout);
                          }*/
                      }
                    }
                }
                command.clear();
        }
    }
    return 0;
}
//connnects to host
int connect_to_host(char *server_ip, int server_port)
{
    //printf("server ip : %s server_port :", *server_ip);
    int fdsocket, len;
    struct sockaddr_in remote_server_addr;

    fdsocket = socket(AF_INET, SOCK_STREAM, 0);
    if(fdsocket < 0)
       perror("Failed to create socket");

    bzero(&remote_server_addr, sizeof(remote_server_addr));
    remote_server_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, server_ip, &remote_server_addr.sin_addr) <= 0)
    {
      fprintf(stderr, "Not in presentation format\n");
      displayException("c");
      fflush(stdout);
      return 0;
    }

    //inet_pton(AF_INET, server_ip, &remote_server_addr.sin_addr);
    remote_server_addr.sin_port = htons(server_port);
    //printf("value of fd socket is %d",fdsocket);
    if(connect(fdsocket, (struct sockaddr*)&remote_server_addr, sizeof(remote_server_addr)) < 0)
      {
        perror("Connect failed");
        return -1;
      }
    return fdsocket;
}

int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
} 

// Splits the input command on the basis of a delimiter
void split(char* s, const char* c,vector<char*>& v) {
    char* temp_s = (char*)malloc(strlen(s)+1);
    strcpy(temp_s,s);
    char* pch = strtok(temp_s,c);
    int i=0;
    while(pch!=NULL){
         v.push_back(pch);
         pch = strtok (NULL, c);   
    }
    //free(temp_s);
}

//Ref:-ubmnc.wordpress.com/2010/09/22/on-getting-the-ip-name-of-a-machine-for-chatty/
//Functionality: used for displaying IP command
char* showIP(){
   
    int sockfd;
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;
    char* myIP=(char*)malloc(INET6_ADDRSTRLEN);
    struct addrinfo hints, *servinfo, *p;
    int status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    //connecting dns of google for ping
    if ((status = getaddrinfo("8.8.8.8", "http", &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }
    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
            perror("UDP: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("UDP: connect");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "UDP: failed to bind socket\n");
        exit(2);
    }
    addrlen = sizeof remoteaddr;
    getsockname(sockfd, (struct sockaddr*)&remoteaddr, &addrlen);

    
    if (remoteaddr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&remoteaddr;
        inet_ntop(AF_INET, &(((struct sockaddr_in *)s)->sin_addr), myIP, addrlen);
    }
    
    //printf("IP_ADDRESS:%s", myIP);

    freeaddrinfo(servinfo); //free the linked list servinfo
    close(sockfd);

    return myIP;
}

//get listening port of the client
int getClientPort(){

}
//print error
void displayException(const char* command_str)
{
  if(strcmp(command_str,"c") == 0)
    {
      cse4589_print_and_log("[%s:ERROR]\n", "RECEIVED");
      cse4589_print_and_log("[%s:END]\n", "RECEIVED");
    }
  else if(strcmp(command_str,"s") == 0)
    {
    cse4589_print_and_log("[%s:ERROR]\n", "RELAYED");
    cse4589_print_and_log("[%s:END]\n", "RELAYED");
    }  
}
