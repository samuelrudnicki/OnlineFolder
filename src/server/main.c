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
#include "../../include/server/secondary.h"
	

int main(int argc, char *argv[])
{
	int sockfd, newsockfd;
	
	socklen_t clilen;
	pthread_t thread_id, thread_replica;
	struct sockaddr_in serv_addr, cli_addr;
	char *ip=malloc(sizeof(MAXNAME));

	
	//Criando a listas
	createList(clientList);
	createServerList(serverList);

	// LENDO ARQUIVO .TXT E CRIANDO LISTA
	FILE *fp;
	long lSize;
	char *buffer;
	char *token;
	fp=fopen("server_list.txt","r");
	if(fp==NULL){
		printf("\nFile Error\n");
		exit(-1);
	}
	// obtem tamanho do arquivo
  	fseek(fp,0,SEEK_END);
 	lSize=ftell(fp);
  	rewind(fp);
	// copia arquivo no buffer
  	buffer=(char*)malloc(sizeof(char)*lSize);	
  	fread(buffer,1,lSize,fp);
	if(strcspn(buffer, "\n")>0)
        buffer[strcspn(buffer, "\n")] = 0;
	// le tokens e adiciona a lista
	token = strtok(buffer,";");
	while(token != NULL && strcmp(token,"")!=0){
		insertServerList(&serverList,token);
		token = strtok(NULL,";");
	}
	/*//imprime lista de servidores (teste) 
	struct serverList *pointer = serverList;
	struct serverList *anotherPointer = serverList;
		do{
		fprintf(stderr,"%s - isPrimary:%d - Previous: %s\n", pointer->serverName, isPrimary(pointer->serverName,&serverList), previousServer(pointer->serverName,&serverList));
		
		pointer=pointer->next;

	}while(pointer!= anotherPointer);
	*/
	//abre conexão replica
	myIp(WANTED_IP, ip);
	if(!isPrimary(ip,&serverList)){
		secondary(primaryServer(&serverList));
	}
		


	if(pthread_create(&thread_replica, NULL, createServer, (void *)SERVERPORT) < 0){
		fprintf(stderr,"ERROR, could not create thread.\n");
		exit(-1);
	}

	printf("Opening Socket...\n");
	//socket para conexão de clientes
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr,"ERROR opening socket.\n");
		exit(-1);
	}
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr.sin_zero), 8);

	printf("Binding Socket...\n");

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr,"ERROR on binding.\n");
		exit(-1);
	}
	
	listen(sockfd, 5);
	
	clilen = sizeof(struct sockaddr_in);

	printf("Accepting new connections...\n");

	while ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) != -1) {
		printf("Connection Accepted\n");

		if(pthread_create(&thread_id, NULL, handleConnection, (void*)&newsockfd) < 0){
			fprintf(stderr,"ERROR, could not create thread.\n");
			exit(-1);
		}
		printf("Handler Assigned\n");

	}
		
	close(sockfd);
	return 0; 
}
