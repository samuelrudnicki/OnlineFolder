#include "../../include/linkedlist/linkedlist.h"

void createList(struct clientList *clientList)
{
	clientList = NULL;
}

void createServerList(struct serverList *serverList)
{
	serverList = NULL;
}

void insertServerList(struct serverList **serverList, char *name){

	struct serverList *server_node= malloc(sizeof(struct serverList));
	struct serverList *pointer = *serverList;

	server_node->next=NULL;
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
		while(pointer->next != NULL)
			pointer= pointer->next;

		pointer->next = server_node;
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

int isPrimary(char *serverName, struct serverList *serverList){

	struct serverList *pointer = serverList;

	while(pointer != NULL){
		if(strcmp(serverName, pointer->serverName) == 0)
			return pointer->isPrimary;
		else
		{
			pointer = pointer->next;
		}
	}
	return 0;
}