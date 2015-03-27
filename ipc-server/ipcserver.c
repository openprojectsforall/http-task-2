#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
#define MAX 10000

//the thread function
void *connection_handler(void *);

void error(char *msg)
{
	printf("%s",msg);
}
 
int main()
{

    int socket_serv , cli_sock , n , *new_sock;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_serv = socket(AF_INET , SOCK_STREAM , 0);

    if (socket_serv == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(12345);
     
    //Bind
    if( bind(socket_serv,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    printf("bind done\n");
     
    //Listen
    listen(socket_serv, 5);
     
   
    //Accept and incoming connection
    printf("Waiting for incoming connections...\n");
    n = sizeof(struct sockaddr_in);

    while( (cli_sock = accept(socket_serv, (struct sockaddr *)&client, (socklen_t*)&n)) )
    {
        puts("Connection accepted");
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = cli_sock;
         
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }
     
    if (cli_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[1000];
     
    
    
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 1024 , 0)) > 0 )
    {
        printf("%s",client_message);
        printf("File to be sent :%s\n",client_message);
        char dir[100]="files/";

        strcat(dir,client_message); 

        FILE *fp;
        char buffer[MAX];

        fp=fopen(dir,"r");
        int n=fread(buffer,1,MAX,fp);

        printf("Sending file\n");
        write(sock,buffer,n);
    }
     
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
    free(socket_desc);
     
    return 0;
}
