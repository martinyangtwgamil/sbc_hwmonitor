#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
 
#include <pthread.h>
#include "hb.h"

#define MAX_CLIENTS (8)
 
static int *listen_sock;
static int service_port = 8888;

void *connection_handler(void *);

void shutdown_server(int code)
{
    int i;
  
    close(*listen_sock);
    printf("Shutdown server\n");
    exit(code);
}


void handle_signal_action(int sig_number)
{
    if (sig_number == SIGINT) {
        printf("SIGINT was catched!\n");
        shutdown_server(EXIT_SUCCESS);
    }
    else if (sig_number == SIGPIPE) {
        printf("SIGPIPE was catched!\n");
        shutdown_server(EXIT_SUCCESS);
    }
}

int setup_signals()
{
    struct sigaction sa;
    sa.sa_handler = handle_signal_action;
    if (sigaction(SIGINT, &sa, 0) != 0) {
        perror("sigaction()");
        return -1;
    }
    if (sigaction(SIGPIPE, &sa, 0) != 0) {
        perror("sigaction()");
        return -1;
    }
  
    return 0;
}
 
int server_start()
{
    int socket_desc , new_socket , c , *new_sock;
    listen_sock = &socket_desc;
    struct sockaddr_in server , client;
    char *message;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(service_port);
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("bind failed");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc,  MAX_CLIENTS);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;
         
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }
     
    if (new_socket<0)
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
    char *message , client_message[2000];
    char patch_file[16] = "Linux patch\n";
    struct heart_beat hb;
    memset(&hb, 0, sizeof(struct heart_beat));
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
	memcpy(&hb, client_message, sizeof(struct heart_beat));
        //Send the patch back to client if version differs, initail version is 0.0.0
        //client hb version have not been define, it should send patch back every time
        if (hb.sbc_version_major != 0 || hb.sbc_version_major!=0 || hb.sbc_version_build != 0)
            write(sock ,  patch_file, strlen(patch_file));
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

int main(int argc , char *argv[])
{
    server_start();
}
