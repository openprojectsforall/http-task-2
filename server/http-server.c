#include <netdb.h> //bzero()
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h> //socket()
#include <arpa/inet.h> //inet_addr
#include <unistd.h> //write
#include <pthread.h> //for threading , link with lpthread

#define MAX 10000

void *connection_handler(void *); //the thread function
void *ipc_connect(char *,void *);

int main(int argv,char argc[]){
    
    struct sockaddr_in server,client;
    
    int server_sock,portno,client_sock,n,*new_sock;
    
    server_sock=socket(AF_INET,SOCK_STREAM,0);
    
    if(server_sock==-1)
    {
        error("error creating socket");
    }
    
    portno=8080;
    
    printf("socket created");
    
    server.sin_family=AF_INET;
    
    server.sin_port=htons(portno);
    
    server.sin_addr.S_un=INADDR_ANY;
    if( bind(server_sock,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        error("bind failed. Error");
        return 1;
    }
    
    printf("binding complete");
    
    listen(server_sock,5);
    
    puts("waiting for the client");
    
    n=sizeof(struct sockaddr_in);
    
    while((client_sock=accept(server_sock,(struct sockaddr *)&client,(socklen_t*)&n)))
    {
        puts("connection accepted");

        pthread_t sniffer_thread;

        new_sock=malloc(1);

        *new_sock=client_sock;

        if( pthread_create( &sniffer_thread , NULL , connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");

    }
    
     if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    
    return 0;

}//main

void *connection_handler(void *sock_desc)
{
    
    int sock=*(int*)sock_desc;

    int read_size;

    char *message,client_message[10000];

    write(sock, "HTTP/1.1 200 OK\n", 16);
    
    write(new_socket, "Content-length: 46\n", 19);
    
    write(new_socket, "Content-Type: text/html\n\n", 25);
    
    write(new_socket, "<html><body><H1>This is used for testing</H1></body></html>",46);

    while((read_size=recv(sock,client_message,1024,0))>0)
    {
        printf("%s",client_message);

        char file_name[100];
        int i=0,j=0;
        char temp[100];

        while(client_message[i]!='\n')
        {
            temp[++j]=client_message[i];
            i++;
        }
        temp[j]='\0';
        j=0;
        for(i=5;temp[i]!=' ';i++)
        {
            file_name[j]=temp[i];
            j++;
        }
        file_name[j]='\0';
        ipc_connect(file_name,(void *)sock);
    
    }//while

    if(read_size == 0)
    {
    puts("Client disconnected");
    fflush(stdout);
    }
    else if(read_size == -1)    
    {
    perror("recv failed");
    }
    //Free the socket pointer
    free(sock_desc);
    return 0;
}

void *ipc_connect(char *filename,void *ipcsock)
{
    int sock = *(int *)ipcsock;
    int ipc_socket,portno;
    
    portno=14789;
    
    struct sockaddr_in ipcsock;
    
    struct hostnet *ipcserver;

    ipc_socket=socket(AF_INET,SOCK_STREAM,0);

    ipcserver=gethostbyname("127.0.0.1");

    if (ipcserver==NULL)
    {
        error("no such server");
        exit(0);
    }

    bzero((char *) &ipcsock, sizeof(ipcsock));

    ipcsock.sin_family=AF_INET;

    bcopy((char *)ipcserver->h_addr,(char *)&ipcsock.sin_addr.s_addr,ipcserver->h_length);

    ipcsock.sin_port=htons(portno);

    if(connect(ipcserver,(struct sockaddr *)&ipcsock,sizeof(ipcsock))<0)
    {
        printf("Error!!\n");
    }

    write(ipc_socket,filename,sizeof(filename));
    
    char buffer[MAX];
    
    int n=recv(ipc_socket,buffer,MAX,0);
    
    if(n<0)
    {
        printf("File not found!!\n");
    }

    write(sock, "HTTP/1.1 200 OK\n", 16);
    
    write(sock, "Content-length: 1000\n", 19);
    
    write(sock, "Content-Type: text/html\n\n", 25);
    
    // server send actual file to client.
    
    printf("Sending file\n");

    write(sock,buffer,sz);

    close(ipc_socket);

}