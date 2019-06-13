#ifndef __linkedlist__
#define __linkedlist__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "../../include/server/server.h"

#define KBYTE 1024
#define DELETE 6
#define DOWNLOADALL 5
#define EXIT 4

#define TRUE 1
#define FALSE 0


void createList(struct clientList *clientList); //Cria a lista
/*
Insere ao final da lista. Retorna 1 em caso de sucesso, 0 em caso de erro
*/
int insertList(struct clientList **clientList, struct client client);
/*
Retorna 1 se vazia, 0 se não vazia.
*/
int isEmpty(struct clientList *clientList);
/*
Encontra um cliente na lista apartir do username do mesmo. Retorna 0 caso não encontre o clinte 
*/
int findNode(char *userName, struct clientList *clientList, struct clientList **client);
//insere servidor em uma lista encadeada
void insertServerList(struct serverList **serverList, char *name);
//inicia a lista encadeada de servidores
void createServerList(struct serverList *serverList);
//1 se é servidor primario, 0 se nao
int isPrimary(char *serverName, struct serverList *serverList);


#endif
