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

    //adiciona _sync_dir ao nome do usuario
    strcpy(userName, argv[1]);
    strcat(userName, _sync_dir);

    if (argc < 4) {
		fprintf(stderr,"usage %s <client name> <host name> <port number>\n", argv[0]);
		exit(-1);
    }

    // Inicia semaforos
    semInit();
	
    frontEnd(userName,argv[2],argv[3]);

    printf("Connection Closed. Exiting Program.\n");
    return 0;
}