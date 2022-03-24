#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include <pthread.h>

#define clientNum 4

//creating an array of client threads and sockets
pthread_t clientThreads[clientNum+1];
int clientSock[clientNum+1];

int clientCount = 0;
char message[2048];

void* boardcast(void* param){
    char mess[2048];
    strcpy(mess,message);
    int sender = *(int *)param;

    //Sending the message to everyone but the sender
    for(int i=1;i<=clientNum;i++){
        if(clientSock[i] != 0 && clientSock[i] != sender){
        // if(clientSock[i] != 0){
            send(clientSock[i],mess,sizeof(mess),0);
        }
    }
    pthread_exit(NULL);
}


void* communication(void* param){
    int sock = *(int *)param;
    
    printf("served by thread : %ld \n ",pthread_self());
    
    char buffer[2048]; 
    bzero(buffer,2048);

    char username[2048];
    bzero(username,sizeof(username));
    read(sock,username,sizeof(username));
    strcat(username," : ");

    while(1){
        bzero(buffer,2048);
        read(sock,buffer,sizeof(buffer));
        printf("%s %s\n",username, buffer);
        
        char msg[2048];
        strcpy(msg,username);
        strcat(msg,buffer);
        
        bzero(message,sizeof(message));

        strncpy(message,msg,sizeof(msg));
        
        pthread_t pid;
        
        if(pthread_create(&pid,NULL,boardcast, (void *)&sock ) != 0){
            printf("Error in creating broadcast thread\n");
        }
        pthread_join(pid,NULL);

        if(strcmp(buffer,"quit\n") == 0){
            break;
        }
    }
    printf("User %s ended connection\n",username);
    close(sock);
    for(int i=1;i<=clientNum;i++){
        if(pthread_self() == clientThreads[i]){
            clientThreads[i] = 0;
            clientSock[i] = 0;
        }
    }
    clientCount--;
    pthread_exit(NULL);
}   


int main(int argc,char *argv[]){
    if(argc != 2){
        printf("Enter Port Number as arguement\n");
        return 1;
    }  
    int port = atoi(argv[1]);
    char *ip = "127.0.0.1";     //Hosting the server on localhost, since clients and server are suppose to run on same computer
    
    int server_sock, new_sock;
    struct sockaddr_in server_addr , client_addr;
    socklen_t addr_size;
    
    char buffer[2048];

    //creating server socket
    server_sock = socket(AF_INET,SOCK_STREAM,0);
    //SOCK_STREAM for TCP
    if(server_sock < 0){
        perror("[-] Socket Error\n");
        exit(1);
    }
    printf("[+] TCP socket created\n");

    //clearing space
    memset(&server_addr, '\0' , sizeof(server_addr));

    //setting up values for socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    //Binding the Server process to port
    int n = bind(server_sock, (struct sockaddr*)&server_addr , sizeof(server_addr) );
    if(n < 0){
        perror("[-] Bind Error\n");
        exit(1);
    }
    printf("[+] Bind to port : %d\n" ,port);

    if (listen(server_sock, clientNum+1) == 0)
        printf("[+] Listening\n");
    else
        printf("[-] Error while listening\n");

    clientCount = 0;

    while(1){
        addr_size = sizeof(server_addr);
        // Extract the first connection in the queue
        new_sock = accept(server_sock,(struct sockaddr*)&client_addr,&addr_size);
        bzero(buffer,sizeof(buffer));
        if(clientCount >= clientNum){
            strcpy(buffer,"500");
            send(new_sock,buffer,sizeof(buffer),0);
            close(new_sock);
            printf("Max Client Limit Reached , a connection was rejected\n");
        }
        else{
            strcpy(buffer,"200");
            send(new_sock,buffer,sizeof(buffer),0);
            clientCount++;
            int i = 1;
            for(i = 1;i<=clientNum;i++){
                if(clientThreads[i] == 0){
                    clientSock[i] = new_sock;
                    break;
                }
            }
            if(pthread_create(&clientThreads[i],NULL,communication, &new_sock ) != 0){
                printf("Error in creating thread\n");
            }
            else{
                printf("New Client Connected with ip : %s and port : %d ",inet_ntoa(client_addr.sin_addr),(int) ntohs(client_addr.sin_port));
            }
        }

    }

    return 0;
}