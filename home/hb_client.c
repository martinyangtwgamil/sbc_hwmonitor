#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <netinet/in.h>
 
#include "hb.h"

#define MAX_RECV (1024)
 
int main(int argc , char *argv[])
{

    struct heart_beat hb;
    int sockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if (sockfd == -1){
        printf("Fail to create a socket.");
    }

    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;

    //localhost test
    info.sin_addr.s_addr = inet_addr("127.0.0.1");
    info.sin_port = htons(8888);


    int err = connect(sockfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1){
        printf("Connection error");
    }


    //Send a hb to server
    char receiveMessage[MAX_RECV] = {0};
    send(sockfd, &hb, sizeof(hb), 0);
    recv(sockfd, receiveMessage, sizeof(receiveMessage),0);

    printf("%s", receiveMessage);
    printf("close Socket\n");
    close(sockfd);
    return 0;
}
