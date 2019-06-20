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



void secondaryServer(char *primaryServerIp,int primaryServerPort){
//TODO: CONECTAR AO SERVIDOR E REPLICAR TUDO O QUE HÁ NELE - PASTAS/ARQUIVOS
   // int initialization = 1;
    //pthread_t thread_inotify, thread_listener, thread_writer, thread_reconnection;
    //struct inotyClient *inotyClient = malloc(sizeof(*inotyClient));
    int serverSockfd;
    struct hostent *server;
    struct sockaddr_in serv_addr;
    char buffer[PAYLOAD_SIZE];
    int status;
    
    server = gethostbyname(primaryServerIp);

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
            if(status == 0){
                removeFromServerList(&serverList,primaryServerIp,primaryServerPort);
                close(serverSockfd);
                serverSockfd = election();
                break;
            }
                
    }

}
int election() {
    createServerRing();
    return 0;
}

void createServerRing(){

    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    pthread_t thread_id;

    printf("Opening Socket... RING\n");
    //socket conexao anel
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr,"ERROR opening socket.\n");
		exit(-1);
	}
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(ELECTIONPORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr.sin_zero), 8);    

	printf("Binding Socket... -- RING\n");

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr,"ERROR on binding.\n");
		exit(-1);
	}
	
	listen(sockfd, 5);
	
	clilen = sizeof(struct sockaddr_in);

	printf("Accepting new connections... -- RING\n");

	while ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) != -1) {
		printf("Connection Accepted -- RING\n");

		if(pthread_create(&thread_id, NULL, listenFromRing, (void*)&newsockfd) < 0){
			fprintf(stderr,"ERROR, could not create thread.\n");
			exit(-1);
		}
        if(pthread_create(&thread_id, NULL, writeToRing, NULL) < 0){
			fprintf(stderr,"ERROR, could not create thread.\n");
			exit(-1);
		}

		printf("Handler Assigned\n");

	}
		
	close(sockfd);
	return 0; 
}

void *listenFromRing(void *socketDescriptor) {
    int sockfd = *(int*)socketDescriptor;
    int status;
    char buffer[PACKET_SIZE];

    while(1) {
        status = read(sockfd,buffer,PACKET_SIZE);

        if(status < 0) {
            printf("ERROR reading from socket -- Ring");
            exit(-1);
        }

        printf("WRITE %s",buffer);
    }
    

}


void *writeToRing() {
    int serverSockfd;
    struct hostent *server;
    struct sockaddr_in serv_addr;
    char buffer[PAYLOAD_SIZE];
    int status;
    struct serverList* previousServerNode;
    previousServerNode = previousServer(ip,myPORT,&serverList);

    server = gethostbyname(previousServerNode->serverName);

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

    
	while (connect(serverSockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0);

    printf("CONECTADO\n");

    while(1);


}
/*
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
*/