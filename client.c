#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include <pthread.h>

char username[20];

void* reader(void *param){
    int sock = *(int *)param;
    char buffer[2048];
    while(1){
        bzero(buffer,2048);
        read(sock,buffer,sizeof(buffer));
        //removing the username tag stuck in stdin 
        //printing broadcast message then again printing username message
        fprintf(stderr,"\r%s%s : ",buffer,username);   
    }
}

void* writer(void* param){
    //username  
    char input[2048] , buffer[2048];
    int sock = *(int *)param;
    printf("Enter Username\n");
    fgets(username,sizeof(username),stdin);
    username[strlen(username)-1] = '\0';
    
    send(sock,username,sizeof(username),0);

    printf("You are connected, type 'quit' to exit\n");

    while(strcmp("quit\n",input) != 0){
        printf("%s : ",username);
        fgets(input,2048,stdin);
        bzero(buffer,2048);
        strcpy(buffer,input);
        send(sock,buffer,sizeof(buffer),0);
    }
    close(sock);
    printf("[+] Connection Closed\n\n");
}

int main(int argc ,char *argv[]){
    if(argc != 3){
        printf("Enter IP address and port as arguement\n");
        return 1;
    }

    char *ip = argv[1]; //localhost , since the client and server are suppose to run on same computer
    
    int port = atoi(argv[2]);

    int sock;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[2048]; //to transfer data between client and server   
    int n;

    //Creating a socket
    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0) {
        perror("[-] Socket error");
        exit(1);
    }
    printf("[+] TCP server socket created \n");

    memset(&addr, '\0', sizeof(addr)); //clearing space

    //configuring socket
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr(ip);

    int res = connect(sock , (struct sockaddr*)&addr , sizeof(addr));
    if(res < 0){
        perror("[-] Connection Error");
        exit(1);
    }
    printf("[+] Connected to Server\n");

    bzero(buffer,sizeof(buffer));
    read(sock,buffer,sizeof(buffer));
    int response = atoi(buffer);
    if(response == 500){
        printf("Max Client Limit Reached,Try Again later\n");
        close(sock);
        return 1;
    }
    pthread_t readerThread,writerThread;
    //Thread for reading broadcast
    if(pthread_create(&readerThread,NULL,reader,&sock) != 0){
        printf("Error while creating reader thread\n");
    }
    
    if(pthread_create(&writerThread,NULL,writer,&sock) != 0){
        printf("Error while creating writer thread\n");
    }
    pthread_join(writerThread,NULL);
    return 0;
}
