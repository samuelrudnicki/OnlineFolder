#include "../../include/linkedlist/linkedlist.h"

void createList(struct clientList *clientList)
{
	clientList = NULL;
}
int isPrimary(char *serverName, struct serverList **serverList){

	struct serverList *pointer = *serverList;
	struct serverList *anotherPointer = *serverList;

	do{
		if(strcmp(serverName, pointer->serverName) == 0)
			return pointer->isPrimary;
		else
		{
			pointer = pointer->next;
		}
	}while(pointer != anotherPointer);
	return 0;
}

char* primaryServer(struct serverList **serverList){
	
	struct serverList *pointer = *serverList;
	struct serverList *anotherPointer = *serverList;

	do{
		if(pointer->isPrimary == TRUE)
			return pointer->serverName;
		else
		{
			pointer = pointer->next;
		}
	}while(pointer != anotherPointer);

	return "there is no primary in tha house";
}
char* previousServer(char *serverName, struct serverList **serverList){
	struct serverList *pointer = *serverList;

	while(pointer != NULL){
		if(strcmp(serverName, pointer->serverName) == 0)
			return pointer->previous->serverName;
		else
		{
			pointer = pointer->next;
		}
	}
	return "not found bitch";
}
void createServerList(struct serverList *serverList)
{
	serverList = NULL;
}
void insertServerList(struct serverList **serverList, char *name){

	struct serverList *server_node= malloc(sizeof(struct serverList));
	struct serverList *pointer = *serverList;
	struct serverList *beginPointer = *serverList;

	//struct serverList *anotherPointer = *serverList;
	server_node->next=server_node;
	server_node->previous=server_node;
	/* if(strcspn(name, "\n")>0)
        name[strcspn(name, "\n")] = 0;*/
	strcpy(server_node->serverName,name);
	server_node->isPrimary=FALSE;

	if(*serverList == NULL)
	{
		server_node->isPrimary=TRUE;
		*serverList = server_node;
		return;
	}
	else
	{
		while(pointer->next != beginPointer)
			pointer= pointer->next;

		pointer->next = server_node;
		pointer->next->previous=pointer;
		pointer->next->next=beginPointer;
		beginPointer->previous=pointer->next;
	}
	return;
}
int insertList(struct clientList **clientList, struct client client)
{
	struct clientList *client_node;
	struct clientList *clientList_aux = *clientList;

	client_node = malloc(sizeof(struct clientList));

	client_node->client = client;
	client_node->next = NULL;

	if (*clientList == NULL)
	{
		*clientList = client_node;
		return 1;
	}
	else
	{
		while(clientList_aux->next != NULL)
			clientList_aux = clientList_aux->next;

		clientList_aux->next = client_node;
	}
	return 0;
}

int isEmpty(struct clientList *clientList)
{
	return clientList == NULL;
}

int findNode(char *userid, struct clientList *clientList, struct clientList **client_node)
{
	struct clientList *clientList_aux = clientList;

	while(clientList_aux != NULL)
	{
		if (strcmp(userid, clientList_aux->client.userName) == 0)
		{
			*client_node = clientList_aux;
			return 1;
		}
		else
			clientList_aux = clientList_aux->next;
	}
	return 0;
}