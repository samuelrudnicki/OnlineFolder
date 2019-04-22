#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include "../../include/client/client.h"
#include "../../include/common/common.h"

int main(int argc, char *argv[])
{
    int sockfd, n;
    int exitCommand = FALSE;
    char command[PACKET_SIZE];
    char response[PACKET_SIZE];
    char *option;
    char *path;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    bzero(command,PACKET_SIZE);

    if (argc < 2) {
		fprintf(stderr,"usage %s hostname\n", argv[0]);
		exit(-1);
    }
	
	server = gethostbyname(argv[1]);
	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(-1);
    }

    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("ERROR opening socket\n");
        exit(-1);
    }
    
	serv_addr.sin_family = AF_INET;     
	serv_addr.sin_port = htons(PORT);    
	bzero(&(serv_addr.sin_zero), 8);

    
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("ERROR connecting\n");
        exit(-1);
    }

    //TODO: Create here new thread to watch folder

    //
    while (exitCommand == FALSE) { 
        printf("\nEnter the Command: ");
        bzero(command, PACKET_SIZE);
        fgets(command, PACKET_SIZE, stdin);

        option = strtok(command," ");
        path = strtok(NULL," ");
        if(path != NULL) {
            path = strtok(path,"\n");
        }

        printf("OPTION: %s\n", option);
        printf("PATH: %s\n", path);
        
        /* write in the socket */
        n = write(sockfd, command, strlen(command));
        if (n < 0) 
            printf("ERROR writing to socket\n");

        bzero(response, PACKET_SIZE);
        
        /* read from the socket */
        n = read(sockfd, response, PACKET_SIZE);
        if (n < 0) 
            printf("ERROR reading from socket\n");

        printf("%s\n",response);

        // Switch for options
        if(strcmp(option,"exit\n") == 0) {
            exitCommand = TRUE;
        } else if (strcmp(option, "upload") == 0) { // upload from path
            
        } else if (strcmp(option, "download") == 0) { // download to exec folder
            
        } else if (strcmp(option, "delete") == 0) { // delete from syncd dir
            
        } else if (strcmp(option, "list_server") == 0) { // list user's saved files on dir
            
        } else if (strcmp(option, "list_client") == 0) { // list saved files on dir
            
        } else if (strcmp(option, "get_sync_dir") == 0) { // creates sync_dir and syncs
            
        }

    }

	close(sockfd);

    printf("Connection Closed. Exiting Program.\n");
    return 0;
}