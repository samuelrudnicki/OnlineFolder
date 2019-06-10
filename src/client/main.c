#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>
#include "../../include/client/client.h"
#include "../../include/common/common.h"


pthread_mutex_t clientMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t writeListenMutex = PTHREAD_MUTEX_INITIALIZER;

int inotifyInitialized = FALSE;

sem_t writerSemaphore;

int exitCommand = FALSE;

int main(int argc, char *argv[])
{
    char _sync_dir[10] = "_sync_dir";
    char userName[CLIENT_NAME_SIZE] ={0};
    pthread_t thread_id, thread_id2;
    struct inotyClient *inotyClient = malloc(sizeof(*inotyClient));
    //adiciona _sync_dir ao nome do usuario
    strcpy(userName, argv[1]);
    strcat(userName, _sync_dir);

    if (argc < 4) {
		fprintf(stderr,"usage %s <client name> <host name> <port number>\n", argv[0]);
		exit(-1);
    }
	
    connectToServer(argv[2],argv[3]);

    // Inicia semaforos
    semInit();

    if(authorization(userName) == TRUE) {
        inotyClient->socket = serverSockfd;
        strcpy(inotyClient->userName, userName);
        checkAndCreateDir(userName);
        deleteAll(userName);
        synchronize(serverSockfd,userName);
        
        if(pthread_create(&thread_id, NULL, inotifyWatcher, (void *) inotyClient) < 0){ // Inotify
            fprintf(stderr,"ERROR, could not create thread.\n");
            exit(-1);
        }
        if(pthread_create(&thread_id2, NULL, listener, NULL) < 0){ // Updates from server
            fprintf(stderr,"ERROR, could not create thread.\n");
            exit(-1);
        }
    }
    
    writer(userName);

	close(serverSockfd);

    printf("Connection Closed. Exiting Program.\n");
    return 0;
}