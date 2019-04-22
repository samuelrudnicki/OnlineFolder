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
    
    }

    printf("Closing Socket %d.\n", newsockfd);
	close(newsockfd);
    return 0;
}