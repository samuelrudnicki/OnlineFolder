#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>
#include "../../include/common/common.h"
#include "../../include/linkedlist/linkedlist.h"
#include "../../include/client/client.h"
#include "../../include/server/secondary.h"



void secondary(char *primaryServer){
//TODO: CONECTAR AO SERVIDOR E REPLICAR TUDO O QUE HÁ NELE - PASTAS/ARQUIVOS
   // int initialization = 1;
    //pthread_t thread_inotify, thread_listener, thread_writer, thread_reconnection;
    //struct inotyClient *inotyClient = malloc(sizeof(*inotyClient));
    int serverSockfd;
    struct hostent *server;
    struct sockaddr_in serv_addr;
    char *buffer= malloc(sizeof(PAYLOAD_SIZE));
    int status;
    
    server = gethostbyname(primaryServer);

	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(-1);
    }
    
    if ((serverSockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("ERROR opening socket\n");
        exit(-1);
    }
    
	serv_addr.sin_family = AF_INET;     
	serv_addr.sin_port = htons(SERVERPORT);
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr_list[0]);
	bzero(&(serv_addr.sin_zero), 8);

    
	if (connect(serverSockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("ERROR connecting\n");
        exit(-1);
    }

    while(1){
        status = read(serverSockfd,buffer,PAYLOAD_SIZE);
            if(status = -1){
                election();
                break;
            }
                
    }

}

void election(){

    int i;
    int sockfd;
    int sockrcv;
    //cria servidor de anel

    printf("Starting election process");

    sockfd = (RING_PORT);

    //conecta o anel
    sockrcv = connectToServerTest(,RING_PORT)
    
    //lẽ de uma lista os valores inseridos e envia para o proximo
    for(i=0;i<RING_MAX_LENGTH;i++)
        sendElectToNext(sockfd,i);

    //quando receber um valor que já tenha em sua lista envia um elected com o maior valor na lista (X)

    //quando o servidor X receber elected X, a eleição acabou e pode se tornar o primario
}
void sendElectedtoNext(int sockfd, int serverNumber){

    char buffer[PAYLOAD_SIZE]={0};
    int status;

    sprintf(buffer,"ELECTED;%d;",serverNumber);

    status=write(sockfd,buffer,PAYLOAD_SIZE);

    if (status < 0) 
        printf("ERROR writing to socket\n");

}

void sendElectToNext(int sockfd, int serverNumber){

    char buffer[PAYLOAD_SIZE]={0};
    int status;

    sprintf(buffer,"ELECT;%d;",serverNumber);

    status=write(sockfd,buffer,PAYLOAD_SIZE);

    if (status < 0) 
        printf("ERROR writing to socket\n");

}

void readElectAndElected(int sockfd){

    char buffer[PAYLOAD_SIZE]= {0};
    int status;
    char *token;
    int i;
    int alreadyReceivedElected=0;

    status=read(sockfd,buffer,PAYLOAD_SIZE);
    if (status < 0) 
            printf("ERROR reading from socket");
    
    token=strtok(buffer,";");
    if(strcmp(token,"ELECT") == 0){
        token=strtok(buffer,NULL);
        serverRing[atoi(token)]=TRUE;
        for(i=0;i<RING_MAX_LENGTH;i++)
            sendElectToNext(sockfd,i);
    }else{
        token=strtok(buffer,NULL);
        for(i=0;i<RING_MAX_LENGTH;i++){
            serverRing[i]=0;
        }
        sendElectedtoNext(sockfd,atoi(token));
    }
}
