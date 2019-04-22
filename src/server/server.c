#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include "../../include/common/common.h"

void *handleConnection(void *socketDescriptor) {
    char buffer[PACKET_SIZE];
    int exitCommand = FALSE;
    int n;
    int newsockfd = *(int*)socketDescriptor;

    //TODO: Receives username from client

    //TODO: get_sync_dir, creates directory, if not created

	while(exitCommand == FALSE) {
        bzero(buffer, PACKET_SIZE);
        /* read from the socket */
        n = read(newsockfd, buffer, PACKET_SIZE);
        if (n < 0) 
            printf("ERROR reading from socket");
        printf("Socket %d - Command: %s\n", newsockfd, buffer);

        if(n == 0) {
            printf("Empty Response.\n");
            exitCommand = TRUE;
        }

        /* write in the socket */
        n = write(newsockfd,"Executing Command...", 22);
        if (n < 0) 
            printf("ERROR writing to socket");


        
        if(strcmp(buffer,"exit\n") == 0) {
            exitCommand = TRUE;
        }
        /*
        else if (strcmp(option, "upload") == 0) { // upload from path
            
        } else if (strcmp(option, "download") == 0) { // download to exec folder
            
        } else if (strcmp(option, "delete") == 0) { // delete from syncd dir
            
        } else if (strcmp(option, "list_server") == 0) { // list user's saved files on dir
            
        } else if (strcmp(option, "list_client") == 0) { // list saved files on dir
            
        } else if (strcmp(option, "get_sync_dir") == 0) { // creates sync_dir_<username> and syncs
            
        }
        */
    
    }

    printf("Closing Socket %d.\n", newsockfd);
	close(newsockfd);
    return 0;
}