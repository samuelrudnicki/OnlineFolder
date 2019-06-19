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



int serverSockfd;
int exitCommand = FALSE;
int serverRing[RING_MAX_LENGTH]={0};
void secundary(char *primaryServer){
//TODO: CONECTAR AO SERVIDOR E REPLICAR TUDO O QUE HÁ NELE - PASTAS/ARQUIVOS
    int initialization = 1;
    pthread_t thread_inotify, thread_listener, thread_writer, thread_reconnection;
    struct inotyClient *inotyClient = malloc(sizeof(*inotyClient));

    while(exitCommand == FALSE){
        if(initialization){
            connectToServer(primaryServer,SERVERPORT);
            
                if(pthread_create(&thread_reconnection, NULL, serverReconnection, NULL) < 0){ // Server reconnection
                    fprintf(stderr,"ERROR, could not create listener thread.\n");
                    exit(-1);
                }
                checkAndCreateDir(userName);
                deleteAll(userName);
                synchronize(serverSockfd,userName);
                
                if(pthread_create(&thread_inotify, NULL, inotifyWatcher, (void *) inotyClient) < 0){ // Inotify
                    fprintf(stderr,"ERROR, could not create inotify thread.\n");
                    exit(-1);
                }
                if(pthread_create(&thread_listener, NULL, listener, NULL) < 0){ // Updates from server
                    fprintf(stderr,"ERROR, could not create listener thread.\n");
                    exit(-1);
                }
                if(pthread_create(&thread_writer, NULL, writer, (void *) userName) < 0){ // Inotify
                    fprintf(stderr,"ERROR, could not create writer thread.\n");
                    exit(-1);
                }

                initialization = 0;
            }
            sem_wait(&reconnectionSemaphore);
            close(serverSockfd);
            connectToServer(newServerIp,newServerPort);
            if(authorization(userName) == TRUE) {
                if(pthread_create(&thread_listener, NULL, listener, NULL) < 0){ // Updates from server
                    fprintf(stderr,"ERROR, could not create listener thread.\n");
                    exit(-1);
                }
            }
        }

        pthread_join(thread_listener,NULL);
        // Se o listener fechar, server respondeu com zero no TCP
        printf("Server failure, waiting for new primary server.\n");
        close(serverSockfd);
    }





void election(){

    int i;
    int sockfd;
    int sockrcv;
    //cria servidor de anel
    sockfd = createServer(RING_PORT);

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
