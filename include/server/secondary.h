#ifndef __secondary__
#define __secondary__
#include <pthread.h>

#define RING_MAX_LENGTH 3
#define RING_PORT 5555
/*
Define funcionamento dos servidores de backup
*/
void secondary(char *primaryServer);
/*
Envia elect para o proximo do anel
*/
void sendElectToNext(int sockfd,int serverNumber);
/*
Inicia processo de eleicao
*/
void election();
/*
Envia elected para o proximo do anel
*/
void sendElectedtoNext(int sockfd, int serverNumber);
/*
Le mensagens elect e elected
*/
void readElectAndElected(int sockfd);
#endif