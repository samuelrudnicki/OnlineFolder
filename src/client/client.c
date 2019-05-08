#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <sys/inotify.h>
#include "../../include/common/common.h"
#include "../../include/linkedlist/linkedlist.h"

void *listener(void *socket){
    //char command[PAYLOAD_SIZE];
    char response[PACKET_SIZE];
    int connectionSocket = *(int*)socket;
    packet incomingPacket;
    bzero(response, PACKET_SIZE);

    while(1){
        read(connectionSocket, response, PAYLOAD_SIZE);
        deserializePacket(&incomingPacket,response);
        strcpy(lastFile,incomingPacket.fileName);

        switch(incomingPacket.type) {
                case TYPE_UPLOAD:
                    download(connectionSocket,incomingPacket.fileName,incomingPacket.clientName,TRUE);
                    break;
                case TYPE_INOTIFY:
                    download(connectionSocket,incomingPacket.fileName,incomingPacket.clientName,TRUE);
                    break;
                default:
                    break;
        }
    }
}