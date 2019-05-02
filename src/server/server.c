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

void *inotifyWatcher(void *pathToWatch){
    //char watch[30];
    //strcpy(watch, (char *) pathToWatch);
    int length;
    int fd;
    int wd;
    char buffer[BUF_LEN];

    fd = inotify_init();

        if ( fd < 0 ) {
        perror( "inotify_init" );
    }

    wd = inotify_add_watch( fd, (char *) pathToWatch, 
                            IN_CLOSE_WRITE | IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);

    while (1) {
        int i = 0;
        length = read( fd, buffer, BUF_LEN );

        if ( length < 0 ) {
            perror( "read" );
        }  

        while ( i < length ) {
            struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
            if ( event->len ) {
                if ( event->mask & IN_CREATE ) {
                    if ( event->mask & IN_ISDIR ) {
                        printf( "The directory %s was created in %s.\n", event->name,(char *) pathToWatch);       
                    }
                    else {
                        printf( "The file %s was created in %s.\n", event->name,(char *) pathToWatch);
                    }
                }
                else if ( event->mask & IN_DELETE ) {
                    if ( event->mask & IN_ISDIR ) {
                        printf( "The directory %s was deleted in %s.\n", event->name,(char *) pathToWatch);       
                    }
                    else {
                        printf( "The file %s was deleted in %s.\n", event->name,(char *) pathToWatch);
                    }
                }
                else if ( event->mask & IN_MODIFY ) {
                    if ( event->mask & IN_ISDIR ) {
                        printf( "The directory %s was modified in %s.\n", event->name,(char *) pathToWatch );
                    }
                    else {
                        printf( "The file %s was modified in %s.\n", event->name,(char *) pathToWatch);
                    }
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }
    ( void ) inotify_rm_watch( fd, wd );
    ( void ) close( fd );
}

void *handleConnection(void *socketDescriptor) {
    char buffer[PACKET_SIZE];
    int exitCommand = FALSE;
    int n;
    int newsockfd = *(int*)socketDescriptor;
    int idUserName;
    char *userName = malloc(sizeof(userName));
    char pathServerUsers[30] = "";
    char pathServerToClient[30] = "../client/";
    pthread_t thread_id;

    //TODO: Receives username from client

    //TODO: get_sync_dir, creates directory, if not created

    //TODO: Create here new thread to watch folder
    // Inotify?
    //

    //TODO: Thread to receive updates from client

    //TODO: Linked list to link logged in clients

    /*************************************/
    //Reads the client name and update/search on the client list.
    bzero(buffer, PACKET_SIZE);
    idUserName = read(newsockfd,buffer,PACKET_SIZE);
    if(idUserName < 0)
        printf("ERROR reading from socket");
    strcpy(userName , buffer);
    strcat(pathServerUsers,buffer);
    strcat(pathServerToClient,buffer);

    struct clientList *client_node = malloc(sizeof(*client_node));//node used to find the username on the list.

    if(!findNode(buffer, clientList, &client_node)){
        appendNewClient(newsockfd, buffer);
        /*
        Cria se necessario a pasta do usuario no cliente e/ou no servidor
        */
        checkAndCreateDir(pathServerUsers);
        checkAndCreateDir(pathServerToClient);
        /*
        Lança as thread que fica olhando o diretorio do cliente e do servidor
        */
        if(pthread_create(&thread_id, NULL, inotifyWatcher, (void *) pathServerUsers) < 0){
			    fprintf(stderr,"ERROR, could not create thread.\n");
			    exit(-1);
		}
        if(pthread_create(&thread_id, NULL, inotifyWatcher, (void *) pathServerToClient) < 0){
			    fprintf(stderr,"ERROR, could not create thread.\n");
			    exit(-1);
		}
        write(newsockfd, "authorized", 11);
    }
    else{
        if(!(updateNumberOfDevices(client_node, newsockfd, INSERTDEVICE) == SUCESS)){
            exitCommand = TRUE;
            printf("There is a connection limit of up to two connected devices.Will close this client device.\n");
            write(newsockfd, "notauthorized", 14);
        }
        else{
            /*
            Cria se necessario a pasta do usuario no cliente e/ou no servidor
            */
            checkAndCreateDir(pathServerUsers);
            checkAndCreateDir(pathServerToClient);
            /*
            Lança as thread que fica olhando o diretorio do cliente e do servidor
            */
            if(pthread_create(&thread_id, NULL, inotifyWatcher, (void *) pathServerUsers) < 0){
			    fprintf(stderr,"ERROR, could not create thread.\n");
			    exit(-1);
		    }
            if(pthread_create(&thread_id, NULL, inotifyWatcher, (void *) pathServerToClient) < 0){
			    fprintf(stderr,"ERROR, could not create thread.\n");
			    exit(-1);
		    }
            write(newsockfd, "authorized", 11);
        }
    }

    /********************************************/
	while(exitCommand == FALSE) {
        bzero(buffer, PACKET_SIZE);
        /* read from the socket */
        n = read(newsockfd, buffer, PACKET_SIZE);
        if (n < 0) 
            printf("ERROR reading from socket");
        printf("Socket %d - Command: %s\n", newsockfd, buffer);

        if(n == 0) {
            printf("Empty response, closing\n");
            if(findNode(userName, clientList, &client_node)){
                if(updateNumberOfDevices(client_node, newsockfd, REMOVEDEVICE)== SUCESS){
                    exitCommand = TRUE;
                }
                else{
                    printf("ERROR updating the client list");
                }
            }
            else{
                printf("ERROR searching the client");
            }
        }

        /* write in the socket */
        n = write(newsockfd,"Executing Command...", 22);
        if (n < 0) 
            printf("ERROR writing to socket");
        
        if(strcmp(buffer,"exit\n") == 0) {
            printf("Exit command, closing\n");
            if(findNode(userName, clientList, &client_node)){
                if(updateNumberOfDevices(client_node, newsockfd, REMOVEDEVICE)== SUCESS){
                    exitCommand = TRUE;
                }
                else{
                    printf("ERROR updating the client list");
                }
            }
            else{
                printf("ERROR searching the client");
            }
        }
        /* else if (strcmp(option, "download") == 0) { // download to exec folder
            
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

void appendNewClient(int socketNewClient, char* userName) {
    struct client *newClient = malloc(sizeof(*newClient));
    newClient->devices[0] = socketNewClient;
    newClient->devices[1] = -1;
    strcpy(newClient->userName , userName);
    insertList(&clientList,*newClient);
}

int updateNumberOfDevices(struct clientList *client_node, int socketNumber, int option){
    if(option == INSERTDEVICE){
        if(client_node->client.devices[0] == FREEDEV)
        {
            client_node->client.devices[0] = socketNumber;
            return 1;
        }
        else if (client_node->client.devices[1] == FREEDEV)
        {
            client_node->client.devices[1] = socketNumber;
            return 1;
        }
        /*TODO: Avisar o client de que a conexão foi finalizada.
          "There is a connection limit of up to two connected devices.""
          Acho que vamos precisar fazer uma thread no client que fica só lendo resposta dos servidor,
          pq se não ele fica presso naquele printf "Enter command"..
        */
        else{ //all devices are busy
            return 0;
        }
    }
    //REMOVE DEVICE
    else{
        for(int device = 0; device <= 1; device++){
            if(client_node->client.devices[device] == socketNumber){
                client_node->client.devices[device] = FREEDEV;
            }
        }
        return 1;
    }

    return 0;
}

int checkAndCreateDir(char *pathName){
    struct stat sb;
    //printf("%s",strcat(pathcomplete, userName));
    if (stat(pathName, &sb) == 0 && S_ISDIR(sb.st_mode)){
        // usuário já tem o diretório com o seu nome
        return 0;
    }
    else{
        if (mkdir(pathName, 0777) < 0){
            //....
            return -1;
        }
        // diretório não existe
        else{
            printf("Creating %s directory...\n", pathName);
            return 0;
        }
    }
}