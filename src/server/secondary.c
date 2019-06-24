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

sem_t listenRingSemaphore;
sem_t writeRingSemaphore;

int highestID = -1;
int participant = FALSE;
int FINISHED = FALSE;
int hasElected = FALSE;
int primaryServerSockfd;

void secondaryServer(char *primaryServerIp,int primaryServerPort){

    int primaryServerSockfd;
    struct hostent *server;
    struct sockaddr_in serv_addr;
    char buffer[PACKET_SIZE];
    packet incomingPacket;
    int status;
    struct serverList* myServerNode;
    myServerNode = findServer(ip,myPORT,&serverList);
    pthread_t thread_replica;

    
    server = gethostbyname(primaryServerIp);

	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(-1);
    }
    
    if ((primaryServerSockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("ERROR opening socket\n");
        exit(-1);
    }
    
	serv_addr.sin_family = AF_INET;     
	serv_addr.sin_port = htons(SERVERPORT);
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr_list[0]);
	bzero(&(serv_addr.sin_zero), 8);

    

    
    while(connect(primaryServerSockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("ERROR connecting\n");
        exit(-1);
    }

    while(1){
        printf("\nWaiting to read from master\n");
        status = read(primaryServerSockfd,buffer,PACKET_SIZE);
        if(status == 0){
            if(myServerNode->next != myServerNode)
                removeFromServerList(&serverList,primaryServerIp,primaryServerPort);
            close(primaryServerSockfd);
            election();
            setPrimary(highestID, &serverList);
            if(myServerNode->isPrimary){
                if(pthread_create(&thread_replica, NULL, createServerPrimary, NULL) < 0){
    		    	fprintf(stderr,"ERROR, could not create thread.\n");
	        		exit(-1);
		        }
            }
            return;
        }

        deserializePacket(&incomingPacket,buffer);

        switch(incomingPacket.type) {
             case TYPE_MIRROR_UPLOAD:
                strcpy(lastFileServer,incomingPacket.fileName);
                downloadCommand(primaryServerSockfd,incomingPacket.fileName,incomingPacket.clientName,FALSE);
                //sync
                //read(primaryServerSockfd, buffer, PACKET_SIZE);
                //expecting ready to upload
                read(primaryServerSockfd, buffer, PACKET_SIZE);
                deserializePacket(&incomingPacket,buffer);
                if(incomingPacket.type == TYPE_UPLOAD_READY){
                    printf("\nDownloading %s...\n", incomingPacket.fileName);
                    download(primaryServerSockfd,incomingPacket.fileName,incomingPacket.clientName,TRUE);
                    printf("\n%s Downloaded.\n", incomingPacket.fileName);
                }
                break;
            case TYPE_MIRROR_DELETE:
                printf("\nDeleting %s...\n", incomingPacket.fileName);
                delete(primaryServerSockfd,incomingPacket.fileName, incomingPacket.clientName);
                //sync   
                //read(primaryServerSockfd, buffer, PACKET_SIZE);
                break;
            case TYPE_NEW_CLIENT:
                printf("\nAdding new client to struct...\n");
                struct clientList *client_node = malloc(sizeof(*client_node));//node used to find the username on the list.
                if (!findNode(incomingPacket.clientName, clientList, &client_node)){
                    appendNewClient(-1, incomingPacket.clientName, incomingPacket.fileName);
                    checkAndCreateDir(incomingPacket.clientName);
                   // sprintf(auth,"%s","authorized");
                   // write(newsockfd, auth, PACKET_SIZE);    
                }else {
                    updateNumberOfDevicesRM(client_node, -1, INSERTDEVICE, incomingPacket.fileName);
                }
                    //sync
                    //read(primaryServerSockfd,buffer,PACKET_SIZE);
                break;
            /*
            case get_sync_dir_server:
                write pro server pedindo os arquivos
                // entra numa função que fica mandando download request: olhar clientSyncServer no client.c
             */
            /*

                // coloca na lista de clientes
             */
            default:
                break;
        }
                
    }


}

void election() {
    if(serverList!=serverList->next){
        sem_init(&listenRingSemaphore,0,0);
        sem_init(&writeRingSemaphore,0,1);
        highestID = -1;
        participant = FALSE;
        FINISHED = FALSE;
        hasElected = FALSE;
        createServerRing();
        sem_close(&listenRingSemaphore);
        sem_close(&writeRingSemaphore);
    }else{
        highestID=serverList->id;
    }
}

void createServerRing(){

    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    pthread_t thread_id, thread_id2;

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

    if(pthread_create(&thread_id, NULL, writeToRing, NULL) < 0){
		fprintf(stderr,"ERROR, could not create thread.\n");
		exit(-1);
	}
	if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) != -1) {
		printf("Connection Accepted -- RING\n");

		if(pthread_create(&thread_id2, NULL, listenFromRing, (void*)&newsockfd) < 0){
			fprintf(stderr,"ERROR, could not create thread.\n");
			exit(-1);
		}

		printf("Handler Assigned\n");

	}
    while(!FINISHED){};

	close(sockfd);
	return; 
}




void *listenFromRing(void *socketDescriptor) {
    int sockfd = *(int*)socketDescriptor;
    int status;
    char buffer[PACKET_SIZE];
    char* electionStatus;
    char* receivedElectionId;
    struct serverList* myServerNode;
    myServerNode = findServer(ip,myPORT,&serverList);

    while(!FINISHED) {
        sem_wait(&listenRingSemaphore);
        if(!FINISHED){
            status = read(sockfd,buffer,PACKET_SIZE);

            if(status < 0) {
                printf("ERROR reading from socket -- Ring");
                exit(-1);
            }

            electionStatus = strtok(buffer,",");

            if(strcmp("ELECTION",electionStatus) == 0) {
                electionStatus = strtok(NULL,",");
                receivedElectionId = electionStatus;
                if(atoi(receivedElectionId) > myServerNode->id) {
                    highestID = atoi(receivedElectionId);
                } else {
                    highestID = myServerNode->id;
                }
            } else if(strcmp("ELECTED",electionStatus) == 0) {
                electionStatus = strtok(NULL,",");
                receivedElectionId = electionStatus;
                highestID = atoi(receivedElectionId);
                // mudar servidor primario
                FINISHED = TRUE;
                hasElected = TRUE;
            }
            electionStatus = NULL;
            receivedElectionId = NULL;
        }
        sem_post(&writeRingSemaphore);
    }
    
    return NULL;
}


void *writeToRing() {
    int serverSockfd;
    struct hostent *server;
    struct sockaddr_in serv_addr;
    char buffer[PACKET_SIZE];
    struct serverList* previousServerNode;
    struct serverList* myServerNode;
    previousServerNode = previousServer(ip,myPORT,&serverList);
    myServerNode = findServer(ip,myPORT,&serverList);
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
	serv_addr.sin_port = htons(ELECTIONPORT);
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr_list[0]);
	bzero(&(serv_addr.sin_zero), 8);

    
	while (connect(serverSockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0);

    printf("RING STARTED\n");

    while(!FINISHED) {
        sem_wait(&writeRingSemaphore);
        if(!hasElected){
            if(highestID < myServerNode->id && !participant){
                participant = TRUE;
                sprintf(buffer,"ELECTION,%d",myServerNode->id);

                write(serverSockfd,buffer,PACKET_SIZE);
            } else if (highestID > myServerNode->id){
                sprintf(buffer,"ELECTION,%d",highestID);

                write(serverSockfd,buffer,PACKET_SIZE);
            } else if(highestID == myServerNode->id && participant){
                sprintf(buffer,"ELECTED,%d",myServerNode->id);

                write(serverSockfd,buffer,PACKET_SIZE);
                //definir como primario
                FINISHED = TRUE;
                hasElected = TRUE;
            }
        } else {
            sprintf(buffer,"ELECTED,%d",highestID);
            write(serverSockfd,buffer,PACKET_SIZE);
        }

        sem_post(&listenRingSemaphore);
    }

    return NULL;
}
void *listenerBackup(void *socketDescriptor){
    char response[PACKET_SIZE];
    int sockfd = *(int*)socketDescriptor;
    packet incomingPacket;
    bzero(response, PACKET_SIZE);
    int listenerStatus;

    while(1){
        
        listenerStatus = read(sockfd, response, PACKET_SIZE);
        
        deserializePacket(&incomingPacket,response);
        

        if(listenerStatus == 0) {
            return NULL;
        }
        
        //pthread_mutex_lock(&listenerInotifyMut);
        //printf("PACKET: %u %u %u %u %s %s %s\n", incomingPacket.type,incomingPacket.seqn,incomingPacket.length,incomingPacket.total_size,incomingPacket.clientName,incomingPacket.fileName,incomingPacket._payload);
        //while(inotifyLength) {
         //   pthread_cond_wait(&noListen, &listenerInotifyMut);
        //}
        listenerStatus = 0;
        //pthread_mutex_lock(&clientMutex);
        switch(incomingPacket.type) {
                case TYPE_UPLOAD:
                    printf("\nDownloading %s...\n", incomingPacket.fileName);
                    download(sockfd,incomingPacket.fileName,incomingPacket.clientName,TRUE);
                    printf("\n%s Downloaded.\n", incomingPacket.fileName);
                    break;
                case TYPE_MIRROR_UPLOAD:
                    strcpy(lastFileServer,incomingPacket.fileName);
                    downloadCommand(sockfd,incomingPacket.fileName,incomingPacket.clientName,FALSE);
                    read(sockfd, response, PACKET_SIZE);
                    deserializePacket(&incomingPacket,response);
                    if(incomingPacket.type == TYPE_UPLOAD_READY){
                        printf("\nDownloading %s...\n", incomingPacket.fileName);
                        download(sockfd,incomingPacket.fileName,incomingPacket.clientName,TRUE);
                        printf("\n%s Downloaded.\n", incomingPacket.fileName);
                    }
                    break;
                case TYPE_INOTIFY_DELETE:
                    strcpy(lastFileServer,incomingPacket.fileName);
                    printf("\nDeleting %s...\n", incomingPacket.fileName);
                    delete(sockfd,incomingPacket.fileName, incomingPacket.clientName);   
                    break;
                case TYPE_DOWNLOAD_READY:
                    printf("\nUploading %s...\n", incomingPacket.fileName);
                    upload(sockfd,clientPath,incomingPacket.clientName,FALSE);
                    printf("\n%s Uploaded.\n", incomingPacket.fileName);
                    break;
                case TYPE_UPLOAD_READY:
                    printf("\nDownloading %s...\n", incomingPacket.fileName);
                    download(sockfd,incomingPacket.fileName,incomingPacket.clientName,FALSE);
                    printf("\n%s Downloaded.\n", incomingPacket.fileName);
                    break;
                default:
                    break;
        }
        //pthread_mutex_unlock(&clientMutex);
        //pthread_mutex_unlock(&writeListenMutex);
        //pthread_cond_signal(&noInotify);
        //checkAndPost(&writerSemaphore);
        //pthread_mutex_unlock(&listenerInotifyMut);
    }
}