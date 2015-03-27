#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFF 1024

void error(char *msg) //function when any function fails
{
    perror(msg);
    exit(0);
}

int main(int argc , char *argv[]) //for reciving command line arguments which consists of portno server address and its port no (for example : 12535 localhost 14789) 
{
	int sockfd, portno, n;//sockfd know as file decriptor stores values hold by socket system call port no holds portno n stores return value of read and write
	struct sockaddr_in serv_addr;//contains internet address included in <netinet.h> 
	
	struct hostent *server;
    
	char newfile[50];

	char buffer[MAX_BUFF];

	if(argc<3)//for checking whether sufficient command line arguments is given or not
    	{

		error("uncomplete command line arguments");
    	
		exit(0);
    	}
	
	portno=atoi(argv[2]);
	
	sockfd=socket(AF_INET,SOCK_STREAM,0);//creates socket takes 3 arguments AF_INET for any two host on the network ,SOCK_STREAM for stream of data ,and 0 to let the operating system chose the appropriate one
	
	if(sockfd<0) 
		error("unable to open the socket");
	
	server=gethostbyname(argv[1]);//for the host name or server name
	
	if (server==NULL)
	{
		error("no such server");
		exit(0);
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));//function bzero() sets all values in a buffer to zero. It takes two arguments, the first is a pointer to the buffer and the second is the size of the buffer.
    	
	serv_addr.sin_family = AF_INET;
    	
	bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    	
	serv_addr.sin_port = htons(portno);// serv_addr is unsigned short sin_port , which contain the port number ,However, instead of simply copying the port number to this field, it is necessary to convert this to network byte order using the function htons() which converts a port number in host byte order to a port number in network byte order
    	
	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
	{
		error("ERROR connecting");
    	}
    	
	printf("Please enter the filename: ");
	
       	bzero(buffer,256);
    
	scanf("%s",&buffer);

	strcpy(newfile,buffer);

	n = write(sockfd,buffer,strlen(buffer));

	if (n < 0)
		error("ERROR writing to socket");

	bzero(buffer,256);

	FILE *fp;

	int recv=0;

	fp=fopen(newfile,"a");

	while(1)
	`{

		recv=read(sockfd,buffer,256);
	
	       	if(recv<=0)
	 		break;
	 
		fwrite(buffer,1,recv,fp);
    	}  
    	
	close(sockfd);
    
	return 0;
}
