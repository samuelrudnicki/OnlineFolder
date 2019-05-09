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

    printf("PACKET: %u %u %u %ul %s %s %s\n", incomingPacket.type,incomingPacket.seqn,incomingPacket.length,incomingPacket.total_size,incomingPacket.clientName,incomingPacket.fileName,incomingPacket._payload);

    while(1){
        read(connectionSocket, response, PACKET_SIZE);
        deserializePacket(&incomingPacket,response);
        strcpy(lastFile,incomingPacket.fileName);

        switch(incomingPacket.type) {
                case TYPE_UPLOAD:
                    download(connectionSocket,incomingPacket.fileName,incomingPacket.clientName,TRUE);
                    bzero(lastFile,100);
                    break;
                case TYPE_INOTIFY:
                    download(connectionSocket,incomingPacket.fileName,incomingPacket.clientName,TRUE);
                    bzero(lastFile,100);                    
                    break;
                case TYPE_DELETE:
                    delete(connectionSocket,incomingPacket.fileName, incomingPacket.clientName);
                    bzero(lastFile,100);                    
                    break;
                case TYPE_INOTIFY_DELETE:
                    delete(connectionSocket,incomingPacket.fileName, incomingPacket.clientName);
                    bzero(lastFile,100);                    
                    break;
                case TYPE_DOWNLOAD_READY:
                    upload(connectionSocket,clientPath,incomingPacket.clientName,FALSE);      
                default:
                    break;
        }
    }
}


void clientUpload(int sockfd, char* path, char* clientName) {
    uint16_t nread;
    char buffer[PAYLOAD_SIZE] = {0};
    uint32_t totalSize;
    int fileSize;
    FILE *fp;
    int status;
    char* fileName;
    char* finalPath = malloc(strlen(path) + strlen(clientName) + 11);
    char serialized[PACKET_SIZE];
    char response[PAYLOAD_SIZE];
    packet packetToUpload;
    int i = 0;

    // Pega o nome do arquivo a partir do path
    fileName = strrchr(path,'/');
    if(fileName != NULL){
        fileName++;
    } else {
        fileName = path;
    }

    strncpy(packetToUpload.fileName,fileName,FILENAME_SIZE);
    strncpy(packetToUpload.clientName,clientName,CLIENT_NAME_SIZE);


    strncpy(finalPath, path, strlen(path) + 1);

    fp = fopen(finalPath,"r");
    if(fp == NULL) {
        printf("ERROR Could not read file.\n");
        return;
    }

    fseek(fp,0,SEEK_END);
    fileSize = ftell(fp);
    fseek(fp,0,SEEK_SET);

    totalSize = fileSize / PAYLOAD_SIZE;

    packetToUpload.total_size = totalSize;

    while ((nread = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        memset(serialized, '\0', sizeof(serialized));

        packetToUpload.type = TYPE_DATA;
        packetToUpload.seqn = i;
        packetToUpload.length = nread;

        strncpy(packetToUpload._payload, buffer, PAYLOAD_SIZE);

        serializePacket(&packetToUpload, serialized);

        status = write(sockfd, serialized, PACKET_SIZE);

        if(status < 0) {
            printf("ERROR writing to socket\n");
            return;
        }

        bzero(response, PAYLOAD_SIZE);

        /* read from the socket */
        status = read(sockfd, response, PAYLOAD_SIZE);
        if(status < 0) {
            printf("ERROR reading from socket\n");
            return;
        }

        printf("%s\n", response);

        i++;
    }

    fclose(fp);
}