#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 

#define portnumber 8888

int tcp_client(char * arg)
{ 
	int sockfd; 
	struct sockaddr_in server_addr; 

	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:Internet;SOCK_STREAM:TCP
	{ 
		fprintf(stderr,"Socket Error:%s\a\n",strerror(errno)); 
		return -1;
		//exit(1); 
	} 

	bzero(&server_addr,sizeof(server_addr)); 
	server_addr.sin_family=AF_INET;          // IPV4
	server_addr.sin_port=htons(portnumber); 
	server_addr.sin_addr.s_addr=inet_addr(arg);  

	if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) 
	{ 
		fprintf(stderr,"Connect Error:%s\a\n",strerror(errno)); 
		return -1;
//		exit(1); 
	} 

	else
	{
		return sockfd;
	}
#if 0
	pthread_create(&id,NULL,read_msg,(void *)&sockfd);

	printf("/******log********/\n");
	printf("/******chat********/\n");

	char cmd[20];

	struct message msg;

	while(1)
	{
		printf("Please input cmd:\n");
		scanf("%s",cmd);

		if(strcmp(cmd,"log") == 0)
		{
			msg.action = 1;

			printf("Please input name:\n");
			scanf("%s",msg.name);

			printf("Please input passwd:\n");
			scanf("%s",msg.passwd);

			write(sockfd,&msg,sizeof(msg));
		}
		else if(strcmp(cmd,"chat") == 0)
		{
			msg.action = 2;

			printf("Please input toname:\n");
			scanf("%s",msg.toname);

			printf("Please input msg:\n");
			scanf("%s",msg.msg);

			write(sockfd,&msg,sizeof(msg));

		}
		else
		{
			printf("cmd is error!\n");
		}
	}
	/* Ω· ¯Õ®—∂ */ 
	close(sockfd); 
	exit(0); 
#endif
} 
